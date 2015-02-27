// =============================================================================
// File Name: KalmanFilter.hpp
// Description: Implements a Kalman filter for single input filtering
// Author: Tyler Veness
// =============================================================================

#ifndef KALMAN_FILTER_HPP
#define KALMAN_FILTER_HPP

#include "FilterBase.hpp"

// m_stateEstimate == xHat, the running estimate of state calculated by filter

class KalmanFilter : public FilterBase {
public:
    KalmanFilter(double Q, double R);

    void update(double input);

    void setQ(double Q);
    void setR(double R);

    void reset();

private:
    // Covariance of error
    double m_P;

    // Process noise variance
    double m_Q;

    // Measurement noise variance
    double m_R;

    // If true, update() sets current estimate to first measurement
    bool m_firstRun;
};

#endif // KALMAN_FILTER_HPP

