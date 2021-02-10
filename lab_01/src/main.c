#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <interp.h>

int main(void)
{
    show_info();
    
    // пользовательский ввод
    FILE *file = get_file();    // получение файла с данными от пользователя
    mode_t mode = get_mode();    // получение вида интерполяции от пользователя
    int n = get_degree();    // получение степени полинома от пользователя

    double x, y;
    if (mode != INVERSE)
        x = get_argument();    // получение значения аргумента от пользователя
    else
        x = 0.0;    // обратная интерполяция - ищем корень, поэтому аргумент = 0.0

    size_t size = 0;
    
    record_t *data = export_to_array(file, &size);
    fclose(file);

    if (!data)
        return EXIT_FAILURE; // выходим, если не удалось выделить память

    sort(data, size, mode);
    size_t start_index = get_config(data, size, mode, x, n);

    switch (mode)
    {
        case NEWTON:
            y = interp_newton(data, x, start_index, n);
            break;

        case HERMITE:
            y = interp_hermite(data, x, start_index, n);
            break;

        case INVERSE:
            y = interp_inverse(data, size, x, start_index, n);
            break;
        
        default:
            break;
    }

    printf("%.3lf", y);
    free(data);
    return EXIT_SUCCESS;
}