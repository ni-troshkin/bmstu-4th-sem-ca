#ifndef __IO_H_

#define __IO_H_

#define LAB_FILE "tests/lab_table.txt"

typedef enum mode
{
    NONE,
    NEWTON,
    HERMITE,
    INVERSE
} mode_t;

#include <stdio.h>

#define STRMAX 256

void show_info(void);
FILE *get_file(void);
mode_t get_mode(void);
int get_degree(void);
double get_argument(void);

#endif  // __IO_H_