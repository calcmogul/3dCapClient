// Copyright (c) Tyler Veness

#include "kalman_filter.hpp"

KalmanFilter::KalmanFilter(double Q, double R) : m_Q(Q), m_R(R) { reset(); }

void KalmanFilter::update(double input) {
    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_last_time;

    // Initialize estimate to measured value
    if (m_first_run) {
        m_state_estimate = input;

        m_first_run = false;
    }

    // Make a prediction - project the error covariance ahead
    m_P += m_Q * m_dt.count();

    /* Calculate the Kalman gain
     *   (m_P + m_R) is S, the estimate error
     */
    double K = m_P / (m_P + m_R);

    /* Update estimate with measurement zk (z_measured)
     *   'input' is zk in this equation
     *   (input - m_stateEstimate) is y, the innovation
     */
    m_state_estimate += K * (input - m_state_estimate);

    // Correct error covariance
    m_P -= K * m_P;

    // Test for NaN (all comparisons with NaN will be false)
    if ((!(m_state_estimate > 0.0)) && (!(m_state_estimate < 0.0))) {
        m_state_estimate = 0.0;
    }

    // Update the previous time for the next delta
    m_last_time = std::chrono::system_clock::now();
}

void KalmanFilter::set_Q(double Q) { m_Q = Q; }

void KalmanFilter::set_R(double R) { m_R = R; }

void KalmanFilter::reset() {
    m_P = 0.0;
    m_first_run = true;

    FilterBase::reset();
}
