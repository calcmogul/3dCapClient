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

    m_avg = (m_avg * (1 - m_adapt)) + (input * m_adapt);
}

void WeightedAverageFilter::reset() {
    m_avg = 0;
}

float WeightedAverageFilter::getEstimate() {
    return m_avg;
}
