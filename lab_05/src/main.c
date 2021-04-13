#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <integral.h>

int main(void)
{
    show_info(); // информация о программе
    double tau = get_argument(); // получение параметра t

    // количество узлов для формулы Симпсона должно быть нечетно
    size_t n_fi = get_nodes("fi");
    // количество узлов для формулы Гаусса
    size_t n_theta = get_nodes("theta");

    // разбиваем направление fi на равные подынтервалы
    double *fi_arr = malloc(sizeof(double) * n_fi);
    if (!fi_arr)
        return EXIT_FAILURE;
    
    fi_arr[0] = 0;
    double step = M_PI_2 / (n_fi - 1);
    for (size_t i = 1; i < n_fi; i++)
        fi_arr[i] = fi_arr[i - 1] + step;
    
    // вычисляем интегралы для каждого fi по формуле Гаусса
    double *fi_integrals = gauss_integral(tau, fi_arr, n_fi, n_theta);
    if (!fi_integrals)
    {
        free(fi_arr);
        return EXIT_FAILURE;
    }

    // вычисление итогового интеграла формулой Симпсона
    double res = 0;
    for (size_t i = 0; i < (n_fi - 1) / 2; i++)
        res += fi_integrals[2 * i] + 4 * fi_integrals[2 * i + 1] \
            + fi_integrals[2 * i + 2];
    
    res *= step / 3.0 / M_PI_4;
    
    printf("%.3lf", res);
    free(fi_integrals);
    free(fi_arr);
    return EXIT_SUCCESS;
}