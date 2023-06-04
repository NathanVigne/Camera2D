#include <QCoreApplication>
#include "levmar.h"
#include <iostream>

#ifndef LM_DBL_PREC
#error Example program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif

/* the following macros concern the initialization of a random number generator for adding noise */
#undef REPEATABLE_RANDOM
#define DBL_RAND_MAX (double) (RAND_MAX)

#ifdef _MSC_VER // MSVC
#include <process.h>
#define GETPID _getpid
#elif defined(__GNUC__) // GCC
#include <sys/types.h>
#include <unistd.h>
#define GETPID getpid
#else
#warning Do not know the name of the function returning the process id for your OS/compiler combination
#define GETPID 0
#endif /* _MSC_VER */

#ifdef REPEATABLE_RANDOM
#define INIT_RANDOM(seed) srandom(seed)
#else
#define INIT_RANDOM(seed) srand((int) GETPID()) // seed unused
#endif

/* Gaussian noise with mean m and variance s, uses the Box-Muller transformation */
double gNoise(double m, double s)
{
    double r1, r2, val;

    r1 = ((double) rand()) / DBL_RAND_MAX;
    r2 = ((double) rand()) / DBL_RAND_MAX;

    val = sqrt(-2.0 * log(r1)) * cos(2.0 * M_PI * r2);

    val = s * val + m;

    return val;
}

/* model to be fitted to measurements: x_i = p[0]*exp(-p[1]*i) + p[2], i=0...n-1 */
void expfunc(double *p, double *x, int m, int n, void *data)
{
    int i;

    for (i = 0; i < n; ++i) {
        x[i] = p[0] * exp(-2.0 * pow(i - p[1], 2) / pow(p[2], 2)) + p[3];
    }
}

/* Jacobian of expfunc() */
void jacexpfunc(double *p, double *jac, int m, int n, void *data)
{
    int i, j;
    double temp = 0;
    /* fill Jacobian row by row */
    for (i = j = 0; i < n; ++i) {
        temp = exp(-2.0 * pow(i - p[1], 2) / pow(p[2], 2));
        jac[j++] = temp;
        jac[j++] = 4 * p[0] / pow(p[2], 2) * (i - p[1]) * temp;
        jac[j++] = 4 * p[0] / pow(p[2], 3) * pow(i - p[1], 2) * temp;
        jac[j++] = 1.0;
    }
}

int main(int argc, char *argv[])
{
    const int n = 1080, m = 4; // 1080 measurements, 4 parameters
    double p[m], x[n], opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int i;
    int ret;

    /* generate some measurement using the exponential model with
   * parameters (5.0, 0.1, 1.0), corrupted with zero-mean
   * Gaussian noise of s=0.1
   */
    INIT_RANDOM(0);
    for (i = 0; i < n; ++i)
        x[i] = (5.0 * exp(-2.0 * pow(i - 100.0, 2) / pow(100, 2)) + 1.0) + gNoise(0.0, 0.1);

    /* initial parameters estimate: (1.0, 0.0, 0.0) */
    p[0] = 2.5;
    p[1] = 200.0;
    p[2] = 50.0;
    p[3] = 0.0;

    /* optimization control parameters; passing to levmar NULL instead of opts reverts to defaults */
    opts[0] = LM_INIT_MU;
    opts[1] = 1E-15;
    opts[2] = 1E-15;
    opts[3] = 1E-20;
    opts[4] = LM_DIFF_DELTA; // relevant only if the finite difference Jacobian version is used

    /* invoke the optimization function */
    ret = dlevmar_der(expfunc,
                      jacexpfunc,
                      p,
                      x,
                      m,
                      n,
                      10000,
                      opts,
                      info,
                      NULL,
                      NULL,
                      NULL); // with analytic Jacobian

    //ret = dlevmar_dif(expfunc, p, x, m, n, 10000, opts, info, NULL, NULL, NULL); // without Jacobian
    printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n",
           info[5],
           info[6],
           info[1],
           info[0]);
    printf("Best fit parameters: %.7g %.7g %.7g %.7g\n", p[0], p[1], p[2], p[3]);

    exit(0);
}
