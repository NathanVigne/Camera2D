#ifndef WORKERFIT_H
#define WORKERFIT_H

#include <QThread>
#include <QTimer>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <iostream>
#include <mutex>

//struct data
//{
//    size_t n;
//    double *t;
//    double *y;
//};

class WorkerFit : public QObject
{
    Q_OBJECT;

public:
    WorkerFit(const size_t N,
              const int P,
              const size_t N_fit,
              std::mutex *mutex,
              QObject *parent = nullptr);
    ~WorkerFit();

    void setMutex(std::mutex *newMutex);
    int setData(double *datas);

    void startLoop(int time_ms);
    void stop();
    void startsingle();

signals:
    void fitEND(double *fitDatas, double *fitParam);

private slots:
    void copyData(double *datas);

private:
    void Fitting();
    void calcFitDatas();
    int initialGuess();

    static int expb_f(const gsl_vector *x, void *data, gsl_vector *f);
    static int expb_df(const gsl_vector *x, void *data, gsl_matrix *f);

    const gsl_multifit_nlinear_type *T;
    gsl_multifit_nlinear_parameters params;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;

    const size_t n;
    const size_t n_fit;
    const size_t p;
    double *m_ydata;
    double *m_xdata;
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
    std::mutex *m_mutex;
    QTimer *timer;
    bool isSingleShot = false;
};

#endif // WORKERFIT_H
