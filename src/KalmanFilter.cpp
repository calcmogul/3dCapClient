// =============================================================================
// File Name: KalmanFilter.cpp
// Description: Implements a Kalman filter for single input filtering
// Author: Tyler Veness
// =============================================================================

#include "KalmanFilter.hpp"

KalmanFilter::KalmanFilter(double Q, double R) :
    m_Q(Q),
    m_R(R) {
    reset();
}

void KalmanFilter::update(double input) {
    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_lastTime;

    // Initialize estimate to measured value
    if (m_firstRun) {
        m_stateEstimate = input;

        m_firstRun = false;
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
    m_stateEstimate += K * (input - m_stateEstimate);

    // Correct error covariance
    m_P -= K * m_P;

    // Test for NaN (all comparisons with NaN will be false)
    if ((!(m_stateEstimate > 0.0)) && (!(m_stateEstimate < 0.0))) {
        m_stateEstimate = 0.0;
    }

    // Update the previous time for the next delta
    m_lastTime = std::chrono::system_clock::now();
}

void KalmanFilter::setQ(double Q) {
    m_Q = Q;
}

void KalmanFilter::setR(double R) {
    m_R = R;
}

void KalmanFilter::reset() {
    m_P = 0.0;
    m_firstRun = true;

    FilterBase::reset();
}

