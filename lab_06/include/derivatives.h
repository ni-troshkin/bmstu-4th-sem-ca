#ifndef __DERIVATIVES_H_

#define __DERIVATIVES_H_

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define EPS 1e-7
#define INIT_LEN 16

#define OK 0
#define ALLOC_ERR -1

typedef struct
{
    double x;
    double y;
} record_t;

record_t *export_to_array(FILE *file, size_t *size);
void one_side_diff_derivative(record_t *data, size_t size);
void center_diff_derivative(record_t *data, size_t size);
void runge_derivative(record_t *data, size_t size);
void alignment_derivative(record_t *data, size_t size);
void second_diff_derivative(record_t *data, size_t size);

#endif  // __DERIVATIVES_H_