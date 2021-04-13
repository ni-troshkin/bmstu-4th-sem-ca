#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <io.h>
#include <integral.h>

// вычисление значения полинома Лежандра степени n в точке х
static double legendre(double x, int n)
{
    if (n == 0)
        return 1;
    
    if (n == 1)
        return x;
    
    double leg0 = 1;
    double leg1 = x;
    double leg2;

    for (int i = 2; i <= n; i++)
    {
        leg2 = ((2 * i - 1) * x * leg1 - (i - 1) * leg0) / i;
        leg0 = leg1;
        leg1 = leg2;
    }

    return leg2;
}

// нахождение корня методом половинного деления
static double bisection(double left, double right, int deg)
{
    double middle = (left + right) / 2;
    do
    {
        if (fabs(legendre(middle, deg)) < EPS)    // EPS = 1e-7
            return middle;
        
        if (legendre(left, deg) * legendre(middle, deg) < 0)
            right = middle;
        else
            left = middle;
        
        middle = (left + right) / 2;
    }
    while (right - left > EPS);
    
    return middle;
}

// нахождение корней полинома Лежандра степени deg
static double *find_roots(size_t deg)
{
    double *roots = malloc(sizeof(double) * deg);    // массив корней
    if (!roots)
        return NULL;
    
    size_t roots_num;
    double step = 2.0 / deg;
    do
    {
        step /= 2.0;    // ширина подынтервала поиска
        roots_num = 0;
        double a = -1;
        double b = a + step;
        while (a < 1)
        {
            if (legendre(a, deg) * legendre(b, deg) < 0)    // разные знаки - корень есть
                roots_num++;
            
            a = b;
            b += step;
        }
    }
    while (roots_num < deg);   // должно найтись deg корней

    double a = -1;
    double b = a + step;
    size_t i = 0;
    while (a < 1 && i < deg)
    {
        if (legendre(a, deg) * legendre(b, deg) < 0)
        {
            roots[i] = bisection(a, b, deg);
            i++;
        }
        
        a = b;
        b += step;
    }

    return roots;
}

static void solve_slae(double **A, double *Y, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
                continue;
            double mult = A[j][i] / A[i][i];
            for (int k = 0; k < n; k++)
                A[j][k] -= mult * A[i][k];
            Y[j] -= mult * Y[i];
        }

    }

    for (int i = 0; i < n; i++)
    {
        double mult = A[i][i];
        for (int j = 0; j < n; j++)
            A[i][j] /= mult;
        Y[i] /= mult;
    }
}

// подынтегральная функция
static double func(double fi, double theta, double tau)
{
    double l_div_R = 2 * cos(theta) / (1 - sin(theta) * sin(theta) * cos(fi) * cos(fi));
    return cos(theta) * sin(theta) * (1 - exp(-tau * l_div_R));
}

static int get_slae_coeffs(double **slae_A, double *slae_Y, size_t n_theta, double *roots)
{
    // здесь будут храниться коэффициенты одной строки матрицы СЛАУ
    // сделано, чтобы не приходилось каждый раз высчитывать степень t_i
    double *coeffs = malloc(n_theta * sizeof(double));
    if (!coeffs)
        return -1;

    // заполнение правой части СЛАУ
    for (size_t i = 0; i < n_theta; i++)
        if (i % 2 == 0)
            slae_Y[i] = 2.0 / (i + 1);
        else
            slae_Y[i] = 0;

    // инициализация нулевой строки матрицы СЛАУ
    for (size_t i = 0; i < n_theta; i++)
        coeffs[i] = 1;
    
    // заполнение матрицы СЛАУ
    for (size_t i = 0; i < n_theta; i++)
        for (size_t j = 0; j < n_theta; j++)
        {
            slae_A[i][j] = coeffs[j];
            coeffs[j] *= roots[j];
        }
    
    free(coeffs);
    return 0;
}

// вычисление интегралов для массива аргументов fi по формуле Гаусса
double *gauss_integral(double tau, double *fi, size_t n_fi, size_t n_theta)
{
    double *roots = find_roots(n_theta);    // корни полинома Лежандра

    // матрицы СЛАУ для нахождения коэффициентов
    double **slae_A = malloc(n_theta * sizeof(double *) + n_theta * n_theta * sizeof(double));
    double *slae_Y = malloc(n_theta * sizeof(double));

    // массив вычисленных значений интеграла при фиксированных fi
    double *integrals = malloc(n_fi * sizeof(double));

    if (!slae_A || !slae_Y || !roots || !integrals)
    {
        free(slae_A);
        free(slae_Y);
        free(roots);
        free(integrals);
        return NULL;
    }
    
    // распределение памяти под матрицу СЛАУ
    double *split = (double *)((char *)slae_A + n_theta * sizeof(double *));

    for (size_t i = 0; i < n_theta; i++)
        slae_A[i] = split + i * n_theta;

    int rc = get_slae_coeffs(slae_A, slae_Y, n_theta, roots);
    
    if (rc)
    {
        free(integrals);
        integrals = NULL;
    }
    else
    {
        // решение СЛАУ
        solve_slae(slae_A, slae_Y, n_theta);

        // преобразование к новому отрезку [0, pi/2]
        for (size_t i = 0; i < n_theta; i++)
            roots[i] = M_PI_4 * (1 + roots[i]);

        // вычисление интегралов для каждого аргумента fi
        for (size_t i = 0; i < n_fi; i++)
        {
            integrals[i] = 0;
            for (size_t j = 0; j < n_theta; j++)
                integrals[i] += slae_Y[j] * func(fi[i], roots[j], tau);
            integrals[i] *= M_PI_4;
        }
    }

    free(slae_A);
    free(roots);
    free(slae_Y);
    return integrals;
}