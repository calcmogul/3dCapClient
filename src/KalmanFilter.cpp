//=============================================================================
//File Name: KalmanFilter.cpp
//Description: Implements a Kalman filter for single input filtering
//Author: Tyler Veness
//=============================================================================

#include "KalmanFilter.hpp"

KalmanFilter::KalmanFilter( double Q , double R ) :
    m_Q( Q ),
    m_R( R )
{
    reset();
}

void KalmanFilter::update( double input ) {
    // Get the current dt since the last call to update()
    m_dt = m_time.getElapsedTime().asSeconds();

    // Initialize estimate to measured value
    if ( m_firstRun ) {
        m_xHat = input;

        m_firstRun = false;
    }

    // Make a prediction - project the error covariance ahead
    m_P += m_Q * m_dt;

    /* Calculate the Kalman gain
     *   (m_P + m_R) is S, the estimate error
     */
    double K = m_P / (m_P + m_R);

    /* Update estimate with measurement zk (z_measured)
     *   'input' is zk in this equation
     *   (input - m_xHat) is y, the innovation
     */
    m_xHat += K * (input - m_xHat);

    // Correct error covariance
    m_P -= K * m_P;

    // Test for NaN (all comparisons with NaN will be false)
    if ( ( !(m_xHat > 0.0) ) && ( !(m_xHat < 0.0) ) ) {
        m_xHat = 0.0;
    }

    // Update the previous time for the next delta
    m_time.restart();
}

void KalmanFilter::setQ( double Q ) {
    m_Q = Q;
}

void KalmanFilter::setR( double R ) {
    m_R = R;
}

double KalmanFilter::getEstimate() {
    return m_xHat;
}

void KalmanFilter::reset() {
    m_xHat = 0.0;
    m_P = 0.0;
    m_dt = 0.0;
    m_time.restart();
    m_firstRun = true;
}
