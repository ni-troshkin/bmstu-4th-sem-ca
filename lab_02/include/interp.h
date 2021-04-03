#ifndef __INTERP_H_

#define __INTERP_H_

typedef struct
{
    double x;
    double y;
    double z;
} record_t;

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define INIT_ROWS 16
#define EPS 1e-6

#define OK 0
#define ALLOC_ERR 1

record_t *export_to_matrix(FILE *file, size_t *rows, size_t *cols);
void sort(record_t *data, size_t rows, size_t cols);
double mult_interp(record_t *data, size_t rows, size_t cols, double x, double y, int nx, int ny);

#endif  // __INTERP_H_