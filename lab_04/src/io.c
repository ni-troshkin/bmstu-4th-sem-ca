#include <stdio.h>
#include <string.h>
#include <io.h>

// Файл с интерфейсом программы (пользовательский ввод)

static void clear_stdinput(void)
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void show_info(void)    // Информация о программе TODO сделать
{
    printf("The program allows to find function value for entered arguments "
        "using spline interpolation\n");
    printf("The function is defined with a table, including x and y\n");
    printf("The argument value MUST be greater or equal to minimum argument\n"
        "in the table and less or equal to maximum argument in the table.\n\n");
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

void get_degree(int *n)
{
    printf("Please enter degree n of approximation polynom: ");
    
    while (scanf("%d", n) != 1 || *n < 0)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }
}

size_t get_index(size_t max_index)
{
    size_t index = 0;
    printf("Please enter index of weight function to change: ");
    
    while (scanf("%zu", &index) != 1 || index > max_index)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }

    return index;
}

double get_weight(void)
{
    double w = 0.0;
    printf("Please enter new weight: ");
    
    while (scanf("%lf", &w) != 1 || w <= 0)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }

    return w;
}

func_t get_func(void)
{
    func_t func;

    do
    {
        printf("Please enter the action to do:\n\n");
        printf("1 - Load new file\n");
        printf("2 - Show current table\n");
        printf("3 - Change node weight\n");
        printf("4 - Calculate approximation function\n");
        printf("0 - Exit\n");
        printf("\nAction: ");
        if (scanf("%u", &func) != 1)
            printf("Incorrect mode. Try again.\n");
        else if (func > APPROX)
            printf("Incorrect mode. Try again.\n");
        else
            return func;
        clear_stdinput();
    }
    while (1);
}
