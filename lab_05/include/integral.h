#ifndef __INTEGRAL_H_

#define __INTEGRAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#define EPS 1e-7

double *gauss_integral(double tau, double *fi, size_t k_fi, size_t k_theta);

#endif  // __INTEGRAL_H_