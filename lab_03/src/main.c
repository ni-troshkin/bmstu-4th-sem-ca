#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <interp.h>

int main(void)
{
    show_info(); // информация о программе
    FILE *file = get_file();    // получение файла с данными от пользователя

    double x1, x2, y1, y2;
    get_arguments(&x1, &x2);    // получение значений аргументов от пользователя

    size_t size = 0;
    record_t *data = export_to_array(file, &size);
    fclose(file);

    if (!data)
        return EXIT_FAILURE; // выходим, если не удалось выделить память

    // сортировка матрицы записей для удобства нахождения конфигурации
    sort(data, size);

    spline_t *splines = calc_splines(data, size);

    y1 = get_spline_value(splines, data, size, x1);
    y2 = get_spline_value(splines, data, size, x2);
    //z = mult_interp(data, rows, cols, x, y, nx, ny);    // сама интерполяция
    printf("%.3lf %.3lf", y1, y2);
    free(splines);
    free(data);
    return EXIT_SUCCESS;
}