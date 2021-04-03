#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <interp.h>

int main(void)
{
    show_info(); // информация о программе
    FILE *file = get_file();    // получение файла с данными от пользователя
    int nx, ny;
    get_degrees(&nx, &ny);    // получение степеней полиномов от пользователя

    double x, y, z;
    get_arguments(&x, &y);    // получение значений аргументов от пользователя

    size_t rows = 0, cols = 0;
    record_t *data = export_to_matrix(file, &rows, &cols);
    fclose(file);

    if (!data)
        return EXIT_FAILURE; // выходим, если не удалось выделить память

    // сортировка матрицы записей для удобства нахождения конфигурации
    sort(data, rows, cols);

    z = mult_interp(data, rows, cols, x, y, nx, ny);    // сама интерполяция
    printf("%.3lf", z);
    free(data);
    return EXIT_SUCCESS;
}