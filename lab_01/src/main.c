#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <interp.h>

int main(void)
{
    show_info();
    
    // пользовательский ввод
    FILE *file = get_file();    // получение файла с данными
    mode_t mode = get_mode();    // получение вида интерполяции
    int n = get_degree();    // получение степени полинома

    double x, y;
    if (mode != INVERSE)
        x = get_argument();    // получение значения аргумента
    else
        x = 0.0;

    size_t size = 0;
    // как назвать тип?
    record_t *data = export_to_array(file, &size);
    fclose(file);

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
            y = interp_inverse(data, x, start_index, n);
            break;
        
        default:
            break;
    }

    printf("%.3lf", y);
    free(data);
    return EXIT_SUCCESS;
}