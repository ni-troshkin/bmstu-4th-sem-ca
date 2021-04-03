#include <stdio.h>
#include <string.h>
#include <io.h>

// Файл с интерфейсом программы (пользовательский ввод)

static void clear_stdinput(void)
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void show_info(void)    // TODO: переписать
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

void get_arguments(double *x1, double *x2)
{
    printf("Please enter two arguments x1, x2 to find y(x1) and y(x2): ");
    
    while (scanf("%lf%lf", x1, x2) != 2)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }
}
