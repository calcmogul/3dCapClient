//=============================================================================
//File Name: WeightedAverageFilter.cpp
//Description: Averages new value with old one using a given weight for the new
//             one [0..1]
//Author: Tyler Veness
//=============================================================================

#include "WeightedAverageFilter.hpp"
#include <cmath>

WeightedAverageFilter::WeightedAverageFilter( float adapt ) {
    m_adapt = adapt;
    reset();
}

void WeightedAverageFilter::update( float input ) {
    // Test for NaN (all comparisons with NaN will be false)
    if ( input != INFINITY && ( !(input > 0.0) ) && ( !(input < 0.0) ) ) {
        return;
    }

    // Get the current dt since the last call to update()
    m_dt = m_time.getElapsedTime().asSeconds();

    m_avg = (m_avg * (1 - m_adapt * m_dt)) + (input * m_adapt * m_dt);

    // Update the previous time for the next delta
    m_time.restart();
}

void WeightedAverageFilter::reset() {
    m_avg = 0;
    m_time.restart();
}

float WeightedAverageFilter::getEstimate() {
    return m_avg;
}
