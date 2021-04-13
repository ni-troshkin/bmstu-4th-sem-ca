#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <approx.h>

int main(void)
{
    show_info(); // информация о программе

    size_t size = 0, bufsize = 0;
    record_t *data = NULL, *buf = NULL;
    func_t func;
    do
    {
        func = get_func();
        switch (func)
        {
            case LOAD:
            {
                // получение файла с данными от пользователя
                FILE *file = get_file();
                // считывание табличной функции в массив
                bufsize = 0;
                buf = export_to_array(file, &bufsize);
                fclose(file);

                if (!buf)
                    return EXIT_FAILURE;
                free(data);
                data = buf;
                size = bufsize;
                break;
            }

            case SHOW: 
            {
                // вывод таблицы
                output_table(data, size);
                break;
            }

            case ADJUST:
            {
                // изменение веса узла
                size_t index = get_index(size - 1);
                double weight = get_weight();
                change_weight(data, index, weight);
                break;
            }
            case APPROX: 
            {
                int n = 0;
                get_degree(&n);    // получение степени полинома от пользователя
                double *coeffs = get_polynom_coeffs(data, size, n);
                plot(coeffs, data, n, size);
                free(coeffs);
                break;
            }
            default: break;
        }
    } while (func);

    free(data);
    return EXIT_SUCCESS;
}