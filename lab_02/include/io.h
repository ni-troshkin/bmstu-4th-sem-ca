#ifndef __IO_H_

#define __IO_H_

#define LAB_FILE "tests/lab_table.txt"

#include <stdio.h>

#define STRMAX 256

void show_info(void);
FILE *get_file(void);
void get_degrees(int *nx, int *ny);
void get_arguments(double *x, double *y);

#endif  // __IO_H_