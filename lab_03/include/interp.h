#ifndef __INTERP_H_

#define __INTERP_H_

typedef struct
{
    double x;
    double y;
} record_t;

typedef struct
{
    double a, b, c, d;
} spline_t;

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define INIT_LEN 16
#define EPS 1e-6

#define OK 0
#define ALLOC_ERR 1

record_t *export_to_array(FILE *file, size_t *size);
void sort(record_t *data, size_t size);
spline_t *calc_splines(record_t *data, size_t size);
double get_spline_value(spline_t *splines, record_t *data, size_t size, double x);

#endif  // __INTERP_H_