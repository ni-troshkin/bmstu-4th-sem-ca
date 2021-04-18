#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <derivatives.h>

int main(void)
{
    show_info(); // информация о программе
    FILE *file = get_file();    // получение файла с данными от пользователя

    size_t size = 0;   // size - количество узлов
    
    // считывание табличной функции в массив
    record_t *data = export_to_array(file, &size);
    fclose(file);

    // проверка успешности выделения памяти
    if (!data)
        return EXIT_FAILURE;
    
    // расчет производных каждым методом и вывод их на экран
    one_side_diff_derivative(data, size);
    center_diff_derivative(data, size);
    runge_derivative(data, size);
    alignment_derivative(data, size);
    second_diff_derivative(data, size);
    
    free(data);
    return EXIT_SUCCESS;
}