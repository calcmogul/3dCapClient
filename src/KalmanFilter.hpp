// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include "FilterBase.hpp"

// m_stateEstimate == xHat, the running estimate of state calculated by filter

/**
 * Implements a Kalman filter for single input filtering
 */
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
