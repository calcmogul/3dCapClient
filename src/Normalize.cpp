// =============================================================================
// File Name: Normalize.hpp
// Description: Normalizes numbers from within a given range defined by a min
//             and max to [0..1]
// Author: Tyler Veness
// =============================================================================

#include "Normalize.hpp"

void Normalize::expandRange(float value) {
    if (value < m_min) {
        m_min = value;
    }

    if (value > m_max) {
        m_max = value;
    }
}

void Normalize::setMinimum(float value) {
    m_min = value;
}

void Normalize::expandMaximum(float value) {
    if (value > m_max) {
        m_max = value;
    }
}

float Normalize::normalize(float value) {
    if (m_min == m_max || m_min == INFINITY) {
        return 0.f;
    }

    float temp = (value - m_min) / (m_max - m_min);

    if (temp < 0.f) {
        return 0.f;
    }
    else if (temp > 1.f) {
        return 1.f;
    }

    return temp;
}

float Normalize::linearize(float value) {
    float normalized = normalize(value);

    if (normalized == 0.f) {
        return 1.f;
    }

    float linear = sqrt(1 / normalized);

    // Normalize inverse square value to [0..1] given a maximum distance range
    linear = (linear - m_minDistance) / (m_maxDistance - m_minDistance);

    if (linear < 0.f) {
        return 0.f;
    }
    else if (linear > 1.f) {
        return 1.f;
    }

    return linear;
}

void Normalize::reset() {
    m_min = INFINITY;
    m_max = -INFINITY;
}

