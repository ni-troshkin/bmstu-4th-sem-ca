#ifndef __APPROX_H_

#define __APPROX_H_

typedef struct
{
    double x;
    double y;
    double rho;
} record_t;

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define INIT_LEN 16
#define EPS 1e-6

#define OK 0
#define ALLOC_ERR 1

record_t *export_to_array(FILE *file, size_t *size);
void sort(record_t *data, size_t size);
void output_table(record_t *data, size_t size);
void change_weight(record_t *data, size_t index, double weight);
double *get_polynom_coeffs(record_t *data, size_t size, int n);
void plot(double *coeffs, record_t *data, int n, size_t size);

#endif  // __APPROX_H_