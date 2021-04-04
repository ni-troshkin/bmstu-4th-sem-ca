#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <approx.h>
// $ gnuplot -p -e "p x*x; p x*x*x + 6*x*x + 6*x + 9"
// set title "Simple plots" font ",20"
// set key left box
// set samples 50
// set style data points
// plot [-10:10] sin(x), atan(x), cos(atan(x))
// plot [-10:10] sin(x), atan(x), cos(atan(x)), "1.dat"

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
                FILE *file = get_file();    // получение файла с данными от пользователя
                // считывание табличной функции в массив
                bufsize = 0;
                buf = export_to_array(file, &bufsize);
                fclose(file);

                if (!buf)
                    return EXIT_FAILURE; // выходим, если не удалось выделить память
                free(data);
                data = buf;
                size = bufsize;
                break;
            }

            case SHOW: 
            {
                output_table(data, size);
                break;
            }

            case ADJUST:
            {
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
                break;
            }
            default: break;
        }
    } while (func);

    free(data);
    return EXIT_SUCCESS;
}