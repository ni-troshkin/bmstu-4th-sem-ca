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
    printf("The program evaluates double integral for a certain\n"
            "value of argument t and number of nodes for each of "
            "two integral arguments. Gauss and Simpson methods are used.");
    printf("(c) Nickolay Troshkin, IU7-46B, BMSTU\n\n");
}

double get_argument(void)
{
    double t;
    printf("Please enter the argument t to find eps(t): ");
    
    while (scanf("%lf", &t) != 1)
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }
    
    return t;
}

size_t get_nodes(const char *arg)
{
    size_t num = 0;
    printf("Please enter number of nodes for %s: ", arg);
    
    while (scanf("%zu", &num) != 1 || num == 0 || (arg[0] == 'f' && num % 2 == 0))
    {
        printf("Incorrect value. Try again\n");
        clear_stdinput();
    }

    return num;
}
