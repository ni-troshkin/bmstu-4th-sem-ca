#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <derivatives.h>

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

// вычисление и вывод на экран левосторонней первой производной
void one_side_diff_derivative(record_t *data, size_t size)
{
    printf("FIRST LEFT SIDE DIFFERENTIAL DERIVATIVE:\n");
    printf("y'(%.3lf) = undefined\n", data[0].x);    // не можем вычислить
    double h = data[1].x - data[0].x;
    for (size_t i = 1; i < size; i++)
    {
        double dy = (data[i].y - data[i - 1].y) / h;
        printf("y'(%.3lf) = %.3lf\n", data[i].x, dy);
    }
    printf("\n");
}

// вычисление и вывод на экран центральной первой производной
void center_diff_derivative(record_t *data, size_t size)
{
    printf("FIRST CENTER DIFFERENTIAL DERIVATIVE:\n");
    printf("y'(%.3lf) = undefined\n", data[0].x);
    double h = data[1].x - data[0].x;
    for (size_t i = 1; i < size - 1; i++)
    {
        double dy = (data[i + 1].y - data[i - 1].y) / 2.0 / h;
        printf("y'(%.3lf) = %.3lf\n", data[i].x, dy);
    }
    printf("y'(%.3lf) = undefined\n", data[size - 1].x);
    printf("\n");
}

// вычисление производной по второй формуле Рунге
void runge_derivative(record_t *data, size_t size)
{
    printf("FIRST RUNGE DERIVATIVE:\n");
    printf("y'(%.3lf) = undefined\n", data[0].x);
    printf("y'(%.3lf) = undefined\n", data[1].x);
    double h = data[1].x - data[0].x;
    for (size_t i = 2; i < size; i++)
    {
        double dy = (3 * data[i].y - 4 * data[i - 1].y + data[i - 2].y) / 2.0 / h;
        printf("y'(%.3lf) = %.3lf\n", data[i].x, dy);
    }
    printf("\n");
}

// вычисление с использованием выравнивающих переменных
void alignment_derivative(record_t *data, size_t size)
{
    printf("FIRST ALIGNMENT DERIVATIVE:\n");
    // вычисление производной линейной функции
    double delta_etha = 1.0 / data[1].y - 1.0 / data[0].y;
    double delta_xee = 1.0 / data[1].x - 1.0 / data[0].x;
    double d_etha = delta_etha / delta_xee;

    // вычисление производной в исходных координатах
    for (size_t i = 0; i < size; i++)
    {
        double dy = d_etha / data[i].x / data[i].x * data[i].y * data[i].y;
        printf("y'(%.3lf) = %.3lf\n", data[i].x, dy);
    }
    printf("\n");
}

// вычисление и вывод на экран второй разностной производной
void second_diff_derivative(record_t *data, size_t size)
{
    printf("SECOND DIFFERENTIAL DERIVATIVE:\n");
    printf("y'(%.3lf) = undefined\n", data[0].x);
    double h = data[1].x - data[0].x;
    for (size_t i = 1; i < size - 1; i++)
    {
        double dy = (data[i - 1].y - 2 * data[i].y + data[i + 1].y) / h / h;
        printf("y'(%.3lf) = %.3lf\n", data[i].x, dy);
    }
    printf("y'(%.3lf) = undefined\n", data[size - 1].x);
    printf("\n");
}
