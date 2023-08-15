#ifndef FIT_H
#define FIT_H

#include <QThread>
#include <QTimer>
#include "mychart.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <iostream>
#include <mutex>

struct data
{
    size_t n;
    double *t;
    double *y;
};

class Fit : public QObject
{
    Q_OBJECT;

public:
    Fit(const size_t N,
        const int P,
        const size_t N_fit,
        const int axe,
        MyChart *chart,
        QObject *parent = nullptr);
    ~Fit();

    // initialisation function (setter)
    void setMutex(std::mutex *newMutex);
    void setChart(MyChart *newChart);

    // controlling the fit
    void startLoop(int time_ms);
    void stop();
    void startsingle();
    void offsetUpdate(int &newOffset);

signals:
    void fitEND(double *fitDatas, double *fitParam, std::mutex *mutex);

private slots:
    void getDatas();

private:
    int initialGuess();
    void Fitting();
    void calcFitDatas();

    static int expb_f(const gsl_vector *x, void *data, gsl_vector *f);
    static int expb_df(const gsl_vector *x, void *data, gsl_matrix *f);

    const gsl_multifit_nlinear_type *T;
    gsl_multifit_nlinear_parameters params;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;

    const size_t n;
    const size_t n_fit;
    const size_t p;
    double *m_vdata;
    double *m_wdata;
    double *m_fitData;
    double *m_fitParam;
    data d;
    double x0, w0, A, b;
    double *param_guess;
    gsl_vector_view init_p;
    gsl_vector *f;

    double chisq, chisq0;
    int status, info;
    size_t i;

    QThread thread;
    std::mutex m_mutex;
    QTimer *timer;
    MyChart *m_chart;
    const int axe_XY;
    int offset = 0;
};

#endif // FIT_H
