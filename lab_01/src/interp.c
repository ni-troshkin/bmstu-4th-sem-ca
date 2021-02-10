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

int cmp_x(const void *x1, const void *x2)
{
    const record_t *rec1 = x1;
    const record_t *rec2 = x2;
    if (fabs(rec1->x - rec2->x) < EPS)
        return 0;
    
    if (rec1->x > rec2->x)
        return 1;
    
    return -1;
}

int cmp_y(const void *x1, const void *x2)
{
    const record_t *rec1 = x1;
    const record_t *rec2 = x2;
    if (fabs(rec1->y - rec2->y) < EPS)
        return 0;
    
    if (rec1->y > rec2->y)
        return 1;
    
    return -1;
}

void sort(record_t *data, size_t size, mode_t mode)
{
    if (mode != INVERSE)
        qsort(data, size, sizeof(record_t), cmp_x);
    else
        qsort(data, size, sizeof(record_t), cmp_y);
}

size_t get_config(record_t *data, size_t size, mode_t mode, double x, int n)
{
    int dots = n + 1;
    if (mode == HERMITE)
        dots = (dots + 1) / 2;
    
    size_t i = 0;
    if (mode == INVERSE)
        while (i < size && data[i].y < x)
            i++;
    else
        while (i < size && data[i].x < x)
            i++;
        
    if (i == size)
        return size - dots;

    if (i < (size_t)dots / 2)
        return 0;
    
    if (size <= (size_t)dots && mode != HERMITE)
        return 0;

    return i - dots / 2;
}

double divided_difference(double y0, double y1, double x0, double x1)
{
    return (y0 - y1) / (x0 - x1);
}

double interp_newton(record_t *data, double x, size_t start_index, int n)
{
    double *main_diffs = malloc((n + 1) * sizeof(double));
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    for (int i = 0; i <= n; i++)
        extra_diffs[i] = data[start_index + i].y;
    
    for (int j = 0; j <= n; j++)
        printf("%.3lf ", extra_diffs[j]);
    printf("\n");
    main_diffs[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = i; j <= n; j++)
            extra_diffs[j - i] = divided_difference(extra_diffs[j - i], extra_diffs[j + 1 - i], data[start_index + j - i].x, data[start_index + j].x);
        main_diffs[i] = extra_diffs[0];
        for (int j = 0; j <= n - i; j++)
            printf("%.3lf ", extra_diffs[j]);
        printf("\n");
    }

    double y = 0.0;

    for (int i = 0; i <= n; i++)
    {
        double prod = main_diffs[i];
        for (int j = 0; j < i; j++)
            prod *= (x - data[start_index + j].x);
        y += prod;
    }

    return y;
}

double interp_hermite(record_t *data, double x, size_t start_index, int n)
{
    double *main_diffs = malloc((n + 1) * sizeof(double));
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    int i;
    for (i = 0; i <= n - 1; i += 2)
        extra_diffs[i] = extra_diffs[i + 1] = data[start_index + i / 2].y;
    
    if (i == n)
        extra_diffs[i] = data[start_index + i / 2].y;
    
    for (int j = 0; j <= n; j++)
        printf("%.3lf ", extra_diffs[j]);
    printf("\n");
    
    main_diffs[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = i; j <= n; j++)
            if (i == 1 && j % 2 == 1)
                extra_diffs[j - i] = data[start_index + j / 2].dy;
            else
                extra_diffs[j - i] = divided_difference(extra_diffs[j - i], extra_diffs[j + 1 - i], data[start_index + (j - i)/2].x, data[start_index + j/2].x);
        main_diffs[i] = extra_diffs[0];
        for (int j = 0; j <= n - i; j++)
            printf("%.3lf ", extra_diffs[j]);
        printf("\n");
    }

    double y = 0.0;

    for (int i = 0; i <= n; i++)
    {
        double prod = main_diffs[i];
        for (int j = 0; j < i; j++)
            prod *= (x - data[start_index + j].x);
        y += prod;
    }

    return y;
}

double interp_inverse(record_t *data, double x, size_t start_index, int n)
{
    double *main_diffs = malloc((n + 1) * sizeof(double));
    double *extra_diffs = malloc((n + 1) * sizeof(double));

    for (int i = 0; i <= n; i++)
        extra_diffs[i] = data[start_index + i].x;
    
    main_diffs[0] = extra_diffs[0];
    for (int i = 1; i <= n; i++)
    {
        for (int j = i; j <= n; j++)
            extra_diffs[j - i] = divided_difference(extra_diffs[j - i], extra_diffs[j + 1 - i], data[start_index + j - i].y, data[start_index + j].y);
        main_diffs[i] = extra_diffs[0];
    }

    double y = 0.0;

    for (int i = 0; i <= n; i++)
    {
        double prod = main_diffs[i];
        for (int j = 0; j < i; j++)
            prod *= (x - data[start_index + j].y);
        y += prod;
    }

    return y;
}