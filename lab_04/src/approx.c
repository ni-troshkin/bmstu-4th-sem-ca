#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <approx.h>

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
        fscanf(file, "%lf", &arr[*size].rho);
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

void output_table(record_t *data, size_t size)
{
    printf("\nCurrent function:\n\n");
    printf(" ----- --------------- --------------- -----------\n");
    printf("|  i  |       x       |       y       |    rho    |\n");

    for (size_t i = 0; i < size; i++)
    {
        printf(" ----- --------------- --------------- -----------\n");
        printf("|%5zu|%15.2f|%15.2f|%11.2f|\n", i, data[i].x, data[i].y, data[i].rho);
    }

    printf(" ----- --------------- --------------- -----------\n");
}

void change_weight(record_t *data, size_t index, double weight)
{
    data[index].rho = weight;
}

static void solve_slae(double **A, double *Y, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
                continue;
            double mult = A[j][i] / A[i][i];
            for (int k = 0; k < n; k++)
                A[j][k] -= mult * A[i][k];
            Y[j] -= mult * Y[i];
        }

    }

    for (int i = 0; i < n; i++)
    {
        double mult = A[i][i];
        for (int j = 0; j < n; j++)
            A[i][j] /= mult;
        Y[i] /= mult;
    }
//     inv_matrix(A);
//     for (size_t i = 0; i < n; i++)
//     {
//         X[i] = 0;
//         for (size_t j = 0; j < n; j++)
//             X[i] += A[i][j] * Y[j];
//     }
}

double *get_polynom_coeffs(record_t *data, size_t size, int n)
{
    n++;
    double **slae_A = malloc(n * sizeof(double *) + n * n * sizeof(double));
    double *slae_Y = malloc(n * sizeof(double));
    // double *slae_X = malloc(n * sizeof(double));

    if (!slae_A || !slae_Y)
    {
        free(slae_A);
        free(slae_Y);
        // free(slae_X);
        return NULL;
    }
    
    double *split = (double *)((char *)slae_A + n * sizeof(double *));
    for (int i = 0; i < n; i++)
        slae_A[i] = split + i * n;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            slae_A[i][j] = 0;
            for (size_t k = 0; k < size; k++)
            {
                double prod = data[k].rho;
                for (int l = 0; l < i + j; l++)
                    prod *= data[k].x;
                slae_A[i][j] += prod;
            }
        }
    
    for (int i = 0; i < n; i++)
    {
        slae_Y[i] = 0;
        for (size_t k = 0; k < size; k++)
        {
            double prod = data[k].rho * data[k].y;
            for (int l = 0; l < i; l++)
                prod *= data[k].x;
            slae_Y[i] += prod;
        }
    }

    solve_slae(slae_A, slae_Y, n);

    free(slae_A);
    //free(slae_Y);
    return slae_Y;
}

void plot(double *coeffs, record_t *data, int n, size_t size)
{
    FILE *script = fopen("img/script", "w");
    FILE *points = fopen("img/points", "w");

    for (size_t i = 0; i < size; i++)
        fprintf(points, "%.2lf %.2lf\n", data[i].x, data[i].y);
    fclose(points);

    fprintf(script, "set terminal pngcairo enhanced font \"arial,10\" "
        "fontscale 1.0 size 600, 400\n"
        "set output \'approx.png\'\n"
        "set key bmargin left horizontal Right noreverse enhanced autotitle box lt black linewidth 1.000 dashtype solid\n"
        "set samples 800, 800\n"
        "set title \"Approximation\"\n "
        "set title  font \",20\" textcolor lt -1 norotate\n"
        "set xrange [ * : * ] noreverse writeback\n"
        "set x2range [ * : * ] noreverse writeback\n"
        "set yrange [ * : * ] noreverse writeback\n"
        "set y2range [ * : * ] noreverse writeback\n"
        "set zrange [ * : * ] noreverse writeback\n"
        "set cbrange [ * : * ] noreverse writeback\n"
        "set rrange [ * : * ] noreverse writeback\n"
        "plot [0:%zu] \'points\', ", size);

    for (int i = 0; i <= n; i++)
    {
        fprintf(script, "%.2lf", coeffs[n - i]);
        for (int j = i; j < n; j++)
            fprintf(script, " * x");
        if (i != n)
            fprintf(script, " + ");
        else
            fprintf(script, "\n");
    }

    fclose(script);
    system("cd img && gnuplot -c script && cd ..");
}