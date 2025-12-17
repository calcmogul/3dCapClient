// Copyright (c) Tyler Veness

#include "weighted_average_filter.hpp"

#include <cmath>

WeightedAverageFilter::WeightedAverageFilter(float adapt) { m_adapt = adapt; }

void WeightedAverageFilter::update(double input) {
    // Test for NaN (all comparisons with NaN will be false)
    if (input != INFINITY && (!(input > 0.0)) && (!(input < 0.0))) {
        return;
    }

    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_last_time;

    m_state_estimate = (m_state_estimate * (1 - m_adapt * m_dt.count())) +
                       (input * m_adapt * m_dt.count());

    // Update the previous time for the next delta
    m_last_time = std::chrono::system_clock::now();
}
