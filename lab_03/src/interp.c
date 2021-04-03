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

// чтение таблицы из файла в матрицу записей
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

// сортировка матрицы по возрастанию аргументов х (по столбцам) и у (по строкам)
void sort(record_t *data, size_t size)
{
    qsort(data, size, sizeof(record_t), cmp_x);
}

spline_t *calc_splines(record_t *data, size_t size)
{
    spline_t *splines = malloc((size + 1) * sizeof(spline_t));
    double *xee = malloc((size + 1) * sizeof(double));
    double *etha = malloc((size + 1) * sizeof(double));

    xee[2] = etha[2] = 0;
    for (size_t i = 2; i < size; i++)
    {
        double D_i = data[i].x - data[i - 1].x;
        double B_i = 2 * (data[i - 2].x - data[i].x);
        double A_i = data[i - 1].x - data[i - 2].x;
        double F_i = 3 * ((data[i].y - data[i - 1].y) / D_i - (data[i - 1].y - data[i - 2].y) / A_i);

        xee[i + 1] = D_i / (B_i - A_i * xee[i]);
        etha[i + 1] = (A_i * etha[i] + F_i) / (B_i - A_i * xee[i]);
    }
    splines[size].c = 0;
    for (size_t i = size - 1; i >= 1; i--)
    {
        splines[i].c = xee[i + 1] * splines[i + 1].c + etha[i + 1];
        splines[i].a = data[i - 1].y;
        double h = data[i].x - data[i - 1].x;
        splines[i].b = (data[i].y - data[i - 1].y) / h - 1 / 3.0 * h * (2 * splines[i].c + splines[i + 1].c);
        splines[i].d = (splines[i + 1].c - splines[i].c) / h / 3.0;
    }

    free(etha);
    free(xee);
    return splines;
}

double get_spline_value(spline_t *splines, record_t *data, size_t size, double x)
{
    size_t i;
    for (i = 0; i < size && x > data[i].x; i++);

    double y = splines[i].a;
    y += splines[i].b * (x - data[i - 1].x);
    y += splines[i].c * (x - data[i - 1].x) *  (x - data[i - 1].x);
    y += splines[i].d *  (x - data[i - 1].x) *  (x - data[i - 1].x) *  (x - data[i - 1].x);

    return y;
}
