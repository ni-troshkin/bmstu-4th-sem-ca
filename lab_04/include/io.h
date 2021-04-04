#ifndef __IO_H_

#define __IO_H_

#define LAB_FILE "tests/lab_table.txt"

#include <stdio.h>

#define STRMAX 256

typedef enum
{
    EXIT,
    LOAD,
    SHOW,
    ADJUST,
    APPROX
} func_t;

void show_info(void);
FILE *get_file(void);
func_t get_func(void);
size_t get_index(size_t max_index);
double get_weight(void);
void get_degree(int *n);

#endif  // __IO_H_