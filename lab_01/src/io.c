#include <stdio.h>
#include <string.h>
#include <io.h>

// Файл с интерфейсом программы (пользовательский ввод)

static void clear_stdinput(void)
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void show_info(void)
{
    printf("The program allows to find function value for entered argument "
        "or the root of the function using inverse interpolation\n");
    printf("The function is defined with a table, including x, y and y\'\n");
    printf("Linear polynomial interpolation is performed with\n");
    printf("Newton polynom, Hermite polynom and inverse interpolation\n");
    printf("The degree of the polynom MUST be less or equal "
        "to number of records in the data file.\n\n");
    printf("(c) Nickolay Troshkin, IU7-46B, BMSTU\n\n");
}

FILE *get_file(void)
{
    char filename[STRMAX];
    FILE *f = NULL;

    do
    {
        printf("Please enter the name of the file with data: ");
        
        if (fgets(filename, STRMAX, stdin) == NULL)
            printf("Couldn't read the name of the file. Try again.\n");
        if (strlen(filename) == STRMAX - 1)
            printf("The file name is too long. Try again.\n");
        filename[strlen(filename) - 1] = '\0';
        f = fopen(filename, "r");
        if (!f)
            printf("The file is not found. Try again.\n");
        else
            return f;
    }
    while (!f);

    return NULL;
}

mode_t get_mode(void)
{
    mode_t mode;

    do
    {
        printf("Please enter the kind of interpolation to use:\n\n");
        printf("1 - Newton polynomial interpolation\n");
        printf("2 - Hermite polynomial interpolation\n");
        printf("3 - Inverse interpolation (for root finding)\n");
        printf("\nMode: ");
        if (scanf("%u", &mode) != 1)
            printf("Incorrect mode. Try again.\n");
        else if (mode > INVERSE || mode == NONE)
            printf("Incorrect mode. Try again.\n");
        else
            return mode;
        clear_stdinput();
    }
    while (1);
}

int get_degree(void)
{
    int n;
    printf("Please enter the degree of the polynom: ");
    
    while (scanf("%d", &n) != 1)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }
    
    return n;
}

double get_argument(void)
{
    double x;
    printf("Please enter the argument x to find y(x): ");
    
    while (scanf("%lf", &x) != 1)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }
    
    return x;
}
