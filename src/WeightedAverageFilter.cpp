// =============================================================================
// File Name: WeightedAverageFilter.cpp
// Description: Averages new value with old one using a given weight for the new
//             one [0..1]
// Author: Tyler Veness
// =============================================================================

#include "WeightedAverageFilter.hpp"
#include <cmath>

WeightedAverageFilter::WeightedAverageFilter(float adapt) {
    m_adapt = adapt;
}

void WeightedAverageFilter::update(double input) {
    // Test for NaN (all comparisons with NaN will be false)
    if (input != INFINITY && (!(input > 0.0)) && (!(input < 0.0))) {
        return;
    }

    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_lastTime;

    m_stateEstimate = (m_stateEstimate * (1 - m_adapt * m_dt.count())) +
                      (input * m_adapt * m_dt.count());

    // Update the previous time for the next delta
    m_lastTime = std::chrono::system_clock::now();
}

