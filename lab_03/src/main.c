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

    size_t size = 0;   // size - количество узлов
    // считывание табличной функции в массив
    record_t *data = export_to_array(file, &size);
    fclose(file);

    if (!data)
        return EXIT_FAILURE; // выходим, если не удалось выделить память

    // сортировка массива записей для удобства
    sort(data, size);

    // вычисление коэффициентов кубических сплайнов
    spline_t *splines = calc_splines(data, size);

    if (!splines)
        return EXIT_FAILURE; // выход, если ошибка выделения памяти

    // нахождение значения функции как значения соответствующего сплайна
    y1 = get_spline_value(splines, data, size, x1);
    y2 = get_spline_value(splines, data, size, x2);
    
    printf("%.3lf %.3lf", y1, y2);
    free(splines);
    free(data);
    return EXIT_SUCCESS;
}