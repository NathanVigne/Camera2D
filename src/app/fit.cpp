#include "fit.h"

Fit::Fit(
    const size_t N, const int P, const size_t N_fit, const int axe, MyChart *chart, QObject *parent)
    : n(N)
    , p(P)
    , n_fit(N_fit)
    , axe_XY(axe)
    , m_chart(chart)
    , QObject(parent)
{
    // Create timer for calling fitting periodically !
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Fit::Fitting);

    // Creates an instance of a Levenberg-Marquardt solver
    // for N data points and P parameters, using suggested defaults
    T = gsl_multifit_nlinear_trust;
    params = gsl_multifit_nlinear_default_parameters();

    // allocate workspace with default parameters
    w = gsl_multifit_nlinear_alloc(T, &params, n, p);

    // Define the function to minimize
    fdf.f = &Fit::expb_f;
    fdf.df = &Fit::expb_df; //&worker::expb_df; //set to NULL for finite-difference Jacobian
    fdf.fvv = NULL;         // not using geodesic acceleration
    fdf.n = n;
    fdf.p = p;

    // Allocate space for data
    m_vdata = new double[N];
    for (int i = 0; i < n; ++i) {
        m_vdata[i] = i;
    }
    m_wdata = new double[N];
    for (int i = 0; i < n; ++i) {
        m_wdata[i] = 0;
    }
    // Allocate space for fit param
    param_guess = new double[P];
    m_fitParam = new double[P];
    m_fitData = new double[N_fit];

    // Move this object to a new thread !
    moveToThread(&thread);
    thread.start();
    std::clog << "Fit :: Constructor. Thread : " << QThread::currentThreadId() << std::endl;
}

Fit::~Fit()
{
    gsl_multifit_nlinear_free(w);
    delete[] m_vdata;
    delete[] m_wdata;
    delete[] m_fitData;
    delete[] m_fitParam;
    std::clog << "Fit :: Thread running ? " << thread.isRunning() << std::endl;
    thread.quit();
    thread.wait();
    std::clog << "Fit :: Thread running ? " << thread.isRunning() << std::endl;
    std::clog << "Fit :: Destructor. Thread : " << std::endl;
}

void Fit::setMutex(std::mutex *newMutex)
{
    //m_mutex = newMutex;
}

void Fit::setChart(MyChart *newChart)
{
    m_chart = newChart;
}

void Fit::startLoop(int time_ms)
{
    timer->start(time_ms);
}

void Fit::stop()
{
    timer->stop();
}

// A revoir la fonction a appeler
void Fit::startsingle()
{
    timer->stop();
    timer->singleShot(100, this, &Fit::Fitting);
}

void Fit::getDatas()
{
    std::scoped_lock lock(*m_chart->mutex_fit(), m_mutex);
    memcpy(m_wdata, m_chart->getD_data(), sizeof(double) * n);

    d.n = n;
    d.t = m_vdata;
    d.y = m_wdata;
    fdf.params = &d;
}

void Fit::Fitting()
{
    //    std::clog << "Start Fit" << std::endl;

    getDatas();

    initialGuess();

    /*
    std::clog << "A      = " << gsl_vector_get(&init_p.vector, 0) << "\n"
              << "x0     = " << gsl_vector_get(&init_p.vector, 0) << "\n"
              << "w0     = " << gsl_vector_get(&init_p.vector, 0) << "\n"
              << "b      = " << gsl_vector_get(&init_p.vector, 0) << std::endl;
    */

    // initialize solver with starting point and weights
    gsl_multifit_nlinear_init(&init_p.vector, &fdf, w);

    // compute initial cost function
    f = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(f, f, &chisq0);

    // solve the system with a maximum of 100 iterations
    const double xtol = 1e-8;
    const double gtol = 1e-8;
    const double ftol = 0.0;
    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, NULL, NULL, &info, w);

    // compute covariance of best fit parameters
    //J = gsl_multifit_nlinear_jac(w);
    //gsl_multifit_nlinear_covar(J, 0.0, covar);

    // compute final cost
    gsl_blas_ddot(f, f, &chisq);

    calcFitDatas();

    /*
    if (info == 1) {
        std::clog << "reason for stopping: "
                  << "small step size" << std::endl;
    } else {
        std::clog << "reason for stopping: "
                  << "small gradient" << std::endl;
    }
    std::clog << "init   |f(x)| = " << sqrt(chisq0) << std::endl;
    std::clog << "final   |f(x)| = " << sqrt(chisq) << std::endl;
    double dof = n - p;
    std::clog << "chisq/dof = " << chisq / dof << std::endl;
    std::clog << "status = " << gsl_strerror(status) << std::endl;
*/
    /* // Print result
#define FIT(i) gsl_vector_get(w->x, i)

    std::clog << "summary from method " << gsl_multifit_nlinear_name(w) << "/"
              << gsl_multifit_nlinear_trs_name(w) << std::endl;

    std::clog << "number of iterations: " << gsl_multifit_nlinear_niter(w) << std::endl;
    std::clog << "function evaluations: " << fdf.nevalf << std::endl;
    std::clog << "Jacobian evaluations: " << fdf.nevaldf << std::endl;
    if (info == 1) {
        std::clog << "reason for stopping: "
                  << "small step size" << std::endl;
    } else {
        std::clog << "reason for stopping: "
                  << "small gradient" << std::endl;
    }
    std::clog << "initial |f(x)| = " << sqrt(chisq0) << std::endl;
    std::clog << "final   |f(x)| = " << sqrt(chisq) << std::endl;

    {
        double dof = n - p;
        double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

        std::clog << "chisq/dof = " << chisq / dof << std::endl;

        std::clog << "A      = " << FIT(0) << std::endl;
        std::clog << "x0     = " << FIT(1) << std::endl;
        std::clog << "w0     = " << FIT(2) << std::endl;
        std::clog << "b      = " << FIT(3) << std::endl;
    }

    std::clog << "status = " << gsl_strerror(status) << std::endl;
*/

    emit fitEND(m_fitData, m_fitParam, &m_mutex);
}

void Fit::calcFitDatas()
{
    // get params
    double A = gsl_vector_get(w->x, 0);
    double x0 = gsl_vector_get(w->x, 1);
    double w0 = abs(gsl_vector_get(w->x, 2));
    double b = gsl_vector_get(w->x, 3);
    m_fitParam[0] = A;
    m_fitParam[1] = x0;
    m_fitParam[2] = w0;
    m_fitParam[3] = b;
    double x = 0.0;

    // calc Fit data
    for (int i = 0; i < n_fit; i++) {
        x = i / ((double) n_fit - 1.0) * (n - 1);
        m_fitData[i] = A * exp(-2 * pow(x - x0, 2) / pow(w0, 2)) + b;
    }
}

// TO DO
// A debugger pour les start nan / inf
int Fit::initialGuess()
{
    // compute the background noise from image edge
    // 5% of width on each side
    double bck = 0.0;
    double N = 0.0;
    for (int i = 0; i < (n / 20); ++i) {
        bck += m_wdata[i] + m_wdata[n - i];
        N = N + 2;
    }
    bck = bck / N;

    // get new vector;
    double yy[n];
    for (int i = 0; i < n; ++i) {
        yy[i] = m_wdata[i] - bck;
    }

    param_guess[0] = gsl_stats_max(m_wdata, 1, n);          // for param : A
    param_guess[3] = gsl_stats_min(m_wdata, 1, n);          // for param : b
    param_guess[1] = gsl_stats_wmean(yy, 1, m_vdata, 1, n); // get centroid
    param_guess[2] = std::abs(gsl_stats_wsd_with_fixed_mean(yy,
                                                            1,
                                                            m_vdata,
                                                            1,
                                                            n,
                                                            x0)
                              * sqrt(2.0)); // bad waist approx!

    init_p = gsl_vector_view_array(param_guess, p);

    return GSL_SUCCESS;
}

int Fit::expb_f(const gsl_vector *x, void *data, gsl_vector *f)
{
    size_t n = ((struct data *) data)->n;
    double *t = ((struct data *) data)->t;
    double *y = ((struct data *) data)->y;

    double A = gsl_vector_get(x, 0);
    double x0 = gsl_vector_get(x, 1);
    double w0 = gsl_vector_get(x, 2);
    double b = gsl_vector_get(x, 3);

    //    std::clog << "-----------" << std::endl;
    //    std::clog << A << " " << x0 << " " << w0 << " " << b << std::endl;

    size_t i;
    for (i = 0; i < n; i++) {
        // Model Yi = A * exp(- 2 * (x_i-x0)^2 / w0^2) + b
        double Yi = A * exp(-2 * pow(t[i] - x0, 2) / pow(w0, 2)) + b;
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

int Fit::expb_df(const gsl_vector *x, void *data, gsl_matrix *J)
{
    size_t n = ((struct data *) data)->n;
    double *t = ((struct data *) data)->t;

    double A = gsl_vector_get(x, 0);
    double x0 = gsl_vector_get(x, 1);
    double w0 = gsl_vector_get(x, 2);
    // double b = gsl_vector_get(x, 3);

    size_t i;
    for (i = 0; i < n; i++) {
        /* Jacobian matrix J(i,j) = dfi / dxj, */
        /* where fi = (Yi - yi)/sigma[i],      */
        /*       Yi = A * exp(-2 (t_i-x0)^2/w0^2) + b  */
        /* and the xj are the parameters (A,x0,w0,b) */
        double e = exp(-2 * pow(t[i] - x0, 2) / pow(w0, 2));
        gsl_matrix_set(J, i, 0, e);
        gsl_matrix_set(J, i, 1, (4 * (t[i] - x0) / pow(w0, 2)) * A * e);
        gsl_matrix_set(J, i, 2, (4 * pow(t[i] - x0, 2) / pow(w0, 3)) * A * e);
        gsl_matrix_set(J, i, 3, 1.0);
    }

    return GSL_SUCCESS;
}
