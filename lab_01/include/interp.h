#ifndef __INTERP_H_

#define __INTERP_H_

typedef struct
{
    double x;
    double y;
    double dy;
} record_t;

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define INIT_LEN 32
#define EPS 1e-6

#define OK 0
#define ALLOC_ERR 1

record_t *export_to_array(FILE *file, size_t *size);
void sort(record_t *data, size_t size, mode_t mode);
size_t get_config(record_t *data, size_t size, mode_t mode, double x, int n);

double interp_newton(record_t *data, double x, size_t start_index, int n);
double interp_hermite(record_t *data, double x, size_t start_index, int n);
double interp_inverse(record_t *data, size_t size, double x, size_t start_index, int n);

#endif  // __INTERP_H_