#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <interp.h>

static int double_capacity(record_t **arr, size_t *capacity)
{
    *capacity *= 2;
    record_t *tmp = realloc(*arr, *capacity * sizeof(record_t));
    if (tmp)
        *arr = tmp;
    else
        return ALLOC_ERR;
    return OK;
}

// чтение таблицы из файла в массив записей
record_t *export_to_array(FILE *file, size_t *size)
{
    record_t *arr = malloc(INIT_LEN * sizeof(record_t));
    if (!arr)
        return NULL;
    *size = 0;
    size_t capacity = INIT_LEN;
    
    while (fscanf(file, "%lf", &arr[*size].x) == 1)
    {
        fscanf(file, "%lf", &arr[*size].y);
        fscanf(file, "%lf", &arr[*size].dy);
        (*size)++;
        if (*size == capacity)
        {
            int rc = double_capacity(&arr, &capacity);
            if (rc)
            {
                free(arr);
                return NULL;
            }
        }
    }

    return arr;    
}
 
// компаратор для сортировки по значению аргумента x
static int cmp_x(const void *x1, const void *x2)
{
    const record_t *rec1 = x1;
    const record_t *rec2 = x2;
    if (fabs(rec1->x - rec2->x) < EPS)
        return 0;
    
    if (rec1->x > rec2->x)
        return 1;
    
    return -1;
}

// компаратор для сортировки по значению функции y(x)
static int cmp_y(const void *x1, const void *x2)
{
    const record_t *rec1 = x1;
    const record_t *rec2 = x2;
    if (fabs(rec1->y - rec2->y) < EPS)
        return 0;
    
    if (rec1->y > rec2->y)
        return 1;
    
    return -1;
}

// сортировка массива в зависимости от типа интерполяции
void sort(record_t *data, size_t size, mode_t mode)
{
    if (mode != INVERSE)
        qsort(data, size, sizeof(record_t), cmp_x);
    else
        qsort(data, size, sizeof(record_t), cmp_y);
}

// функция получает первый индекс в упорядоченном массиве записей, 
// с которого начинаются узлы выбранной конфигурации
size_t get_config(record_t *data, size_t size, mode_t mode, double x, int n)
{
    int dots = n + 1;    // количество записей, которое требуется для полинома
    if (mode == HERMITE)
        dots = (dots + 1) / 2;

    size_t i = 0;
    // поиск первого индекса, по которому значение аргумента больше х
    if (mode == INVERSE)
        while (i < size && data[i].y < x)
            i++;
    else
        while (i < size && data[i].x < x)
            i++;

    if (i == size)
        return size - dots;  // нижняя часть таблицы

    if (i < (size_t)dots / 2)
        return 0;  // верхняя часть таблицы

    if (size <= (size_t)dots && mode != HERMITE)
        return 0;

    return i - dots / 2;  // найдется поровну точек слева и справа от х
}

static double divided_difference(double y0, double y1, double x0, double x1)
{
    return (y0 - y1) / (x0 - x1); // возвращает разделенную разность по 4 аргументам
}

// получение значения полинома степени n с заданными коэффициентами
// при заданном значении аргумента
static double get_polynom_value(double *factors, int n, double x, 
record_t *data, size_t start_index)
{
    double y = 0.0;

    for (int i = 0; i <= n; i++)
    {
        double prod = factors[i];
        for (int j = 0; j < i; j++)
            prod *= (x - data[start_index + j].x);
        y += prod;
    }

    return y;
}

// линейная интерполяция с помощью полинома Ньютона
double interp_newton(record_t *data, double x, size_t start_index, int n)
{
    // в этом массиве сохраняются множители, которые будут в полеученном полиноме
    double *factors = malloc((n + 1) * sizeof(double));

    // дополнительные промежуточные разности, из них получаются разности следующих порядков
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    for (int i = 0; i <= n; i++)
        extra_diffs[i] = data[start_index + i].y;

    factors[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j <= n - i; j++)
        // из ранее вычисленных разностей получаем новую разделенную разность
            extra_diffs[j] = divided_difference(extra_diffs[j], extra_diffs[j + 1],
                data[start_index + j].x, data[start_index + j + i].x);

        factors[i] = extra_diffs[0];
    }

    return get_polynom_value(factors, n, x, data, start_index);
}

// линейная интерполяция с помощью полинома Эрмита
double interp_hermite(record_t *data, double x, size_t start_index, int n)
{
    double *factors = malloc((n + 1) * sizeof(double));
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    int i;
    for (i = 0; i <= n - 1; i += 2)
        extra_diffs[i] = extra_diffs[i + 1] = data[start_index + i / 2].y;
    
    if (i == n)
        extra_diffs[i] = data[start_index + i / 2].y;
    
    factors[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j <= n - i; j++)
            if (i == 1 && j % 2 == 0)
                extra_diffs[j] = data[start_index + j / 2].dy;
            else
                extra_diffs[j] = divided_difference(extra_diffs[j], extra_diffs[j + 1],
                    data[start_index + j / 2].x, data[start_index + (j + i) / 2].x);

        factors[i] = extra_diffs[0];
    }

    return get_polynom_value(factors, n, x, data, start_index);
}

// получение значения с помощью обратной интерполяции
double interp_inverse(record_t *data, size_t size, double x, size_t start_index, int n)
{
     // меняем местами поля x и y в записях
    for (size_t i = 0; i < size; i++)
    {
        double temp = data[i].x;
        data[i].x = data[i].y;
        data[i].y = temp;
    }

    // x = 0.0 - это предусмотрено в функции main()
    return interp_newton(data, x, start_index, n);
}