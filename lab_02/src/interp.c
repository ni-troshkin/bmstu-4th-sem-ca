#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <interp.h>

static int double_capacity(record_t **matr, size_t *capacity, size_t *cols)
{
    *capacity *= 2;
    record_t *tmp = realloc(*matr, *capacity * (*cols) * sizeof(record_t));
    if (tmp)
        *matr = tmp;
    else
        return ALLOC_ERR;
    return OK;
}

// чтение таблицы из файла в матрицу записей
record_t *export_to_matrix(FILE *file, size_t *rows, size_t *cols)
{
    fscanf(file, "%zu", cols);
    record_t *matr = malloc(INIT_ROWS * (*cols) * sizeof(record_t));
    if (!matr)
        return NULL;
    *rows = 0;
    size_t capacity = INIT_ROWS;

    double *x_s = malloc(*cols * sizeof(record_t));
    if (!x_s)
    {
        free(matr);
        return NULL;
    }

    for (size_t i = 0; i < *cols; i++)
        fscanf(file, "%lf", &x_s[i]);
    
    double curr_y;

    while (fscanf(file, "%lf", &curr_y) == 1)
    {
        for (size_t i = 0; i < *cols; i++)
        {
            fscanf(file, "%lf", &(matr[*rows * (*cols) + i].z));
            matr[*rows * (*cols) + i].x = x_s[i];
            matr[*rows * (*cols) + i].y = curr_y;
        }
        (*rows)++;
        if (*rows == capacity)
        {
            int rc = double_capacity(&matr, &capacity, cols);
            if (rc)
            {
                free(matr);
                free(x_s);
                return NULL;
            }
        }
    }

    free(x_s);
    return matr;    
}

static void swap_rows(record_t *data, size_t i, size_t j, size_t cols)
{
    for (size_t k = 0; k < cols; k++)
    {
        record_t tmp = data[i * cols + k];
        data[i * cols + k] = data[j * cols + k];
        data[j * cols + k] = tmp;
    }
}

static void sort_rows(record_t *data, size_t rows, size_t cols)
{
    for (size_t i = 0; i < rows - 1; i++)
        for (size_t j = 0; j < rows - i - 1; j++)
            if (data[j * cols].y > data[(j + 1) * cols].y)
                swap_rows(data, j, j + 1, cols);
}

static void swap_cols(record_t *data, size_t i, size_t j, size_t rows, size_t cols)
{
    for (size_t k = 0; k < rows; k++)
    {
        record_t tmp = data[k * cols + i];
        data[k * cols + i] = data[k * cols + j];
        data[k * cols + j] = tmp;
    }
}

static void sort_cols(record_t *data, size_t rows, size_t cols)
{
    for (size_t i = 0; i < cols - 1; i++)
        for (size_t j = 0; j < cols - i - 1; j++)
            if (data[j].x > data[j + 1].x)
                swap_cols(data, j, j + 1, rows, cols);
}

// сортировка матрицы по возрастанию аргументов х (по столбцам) и у (по строкам)
void sort(record_t *data, size_t rows, size_t cols)
{
    sort_rows(data, rows, cols);
    sort_cols(data, rows, cols);
}

// функция получает первый индекс в упорядоченной матрице записей, 
// с которого начинаются строки выбранной конфигурации
static size_t get_y_config(record_t *data, size_t rows, size_t cols, double y, int ny)
{
    int count_y = ny + 1;

    size_t i = 0;
    // поиск первой строки, по которой значение аргумента y больше введенного у
    while (i < rows && data[i * cols].y < y)
        i++;

    if (i == rows)
        return rows - count_y;  // нижняя часть таблицы

    if (i < (size_t)count_y / 2)
        return 0;  // верхняя часть таблицы

    if (rows <= (size_t)count_y)
        return 0;

    return i - count_y / 2;  // найдется поровну точек сверху и снизу от у
}

// функция получает первый индекс в упорядоченной матрице записей, 
// с которого начинаются столбцы выбранной конфигурации
static size_t get_x_config(record_t *data, size_t cols, double x, int nx)
{
    int count_x = nx + 1;

    size_t i = 0;
    // поиск первого столбца, по которому значение аргумента x больше введенного x
    while (i < cols && data[i].x < x)
        i++;

    if (i == cols)
        return cols - count_x;  // левые столбцы таблицы

    if (i < (size_t)count_x / 2)
        return 0;  // правые столбцы таблицы

    if (cols <= (size_t)count_x)
        return 0;

    return i - count_x / 2;  // найдется поровну точек слева и справа от х
}

static double divided_difference(double y0, double y1, double x0, double x1)
{
    return (y0 - y1) / (x0 - x1); // возвращает разделенную разность по 4 аргументам
}

// получение значения полинома степени n с заданными коэффициентами
// при заданном значении аргумента
static double get_polynom_value(double *factors, int n, double x, 
record_t *data, size_t start_index, int flag)
{
    // флаг flag равен 1, если собираем полином, зависящий от х при
    // фиксированном у, иначе полином, зависящий от y
    double z = 0.0;

    for (int i = 0; i <= n; i++)
    {
        double prod = factors[i];
        for (int j = 0; j < i; j++)
            if (flag == 1)
                prod *= (x - data[start_index + j].x);
            else
                prod *= (x - data[start_index + j].y);
        z += prod;
    }

    free(factors);
    return z;
}

// линейная интерполяция с помощью полинома Ньютона
static double interp_newton(record_t *data, size_t cols, double x, size_t start_index, int n, int row)
{
    // в этом массиве сохраняются множители, которые будут в полученном полиноме
    double *factors = malloc((n + 1) * sizeof(double));

    // дополнительные промежуточные разности, из них получаются разности следующих порядков
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    // row = -1 означает, что передана не матрица, а массив со значениями, 
    // полученными при интерполяции по строкам, в нем отсчитывать
    // количество строк не нужно
    for (int i = 0; i <= n; i++)
        if (row != -1)
            extra_diffs[i] = data[start_index + row * cols + i].z;
        else
            extra_diffs[i] = data[start_index + i].z;

    factors[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j <= n - i; j++)
        // из ранее вычисленных разностей получаем новую разделенную разность
        // в зависимости от того, интерполируем по строкам или по столбцу
            if (row != -1)
                extra_diffs[j] = divided_difference(extra_diffs[j], extra_diffs[j + 1],
                    data[start_index + j + row * cols].x, data[start_index + j + i + row * cols].x);
            else
                extra_diffs[j] = divided_difference(extra_diffs[j], extra_diffs[j + 1],
                    data[start_index + j].y, data[start_index + j + i].y);

        factors[i] = extra_diffs[0];
    }

    free(extra_diffs);

    if (row != -1)
        return get_polynom_value(factors, n, x, data, start_index + row * cols, 1);
    
    return get_polynom_value(factors, n, x, data, start_index, 0);
}

double mult_interp(record_t *data, size_t rows, size_t cols, double x, double y, int nx, int ny)
{
    // выясняем, по каким строкам и столбцам располагаются ближайшие к аргументам значения,
    // чтобы по ним интерполировать табличную функцию
    size_t start_y = get_y_config(data, rows, cols, y, ny);
    size_t start_x = get_x_config(data, cols, x, nx);

    // в этом массиве будут записываться значения z(x, y_i) для фиксированных значений y
    record_t *func_y_fixed = malloc((ny + 1) * sizeof(record_t));

    for (size_t i = start_y; i <= ny + start_y; i++)
    {
        func_y_fixed[i - start_y].y = data[i * cols].y;
        // интерполяция по строке для фиксированного у
        func_y_fixed[i - start_y].z = interp_newton(data, cols, x, start_x, nx, i);
    }
    
    // после интерполяции по строкам интерполируем по полученным значениям переменной у
    double z = interp_newton(func_y_fixed, ny + 1, y, 0, ny, -1);
    free(func_y_fixed);
    
    return z;
}