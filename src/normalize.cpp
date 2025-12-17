// Copyright (c) Tyler Veness

#include "normalize.hpp"

void Normalize::expand_range(float value) {
    if (value < m_min) {
        m_min = value;
    }

    if (value > m_max) {
        m_max = value;
    }
}

void Normalize::set_minimum(float value) { m_min = value; }

void Normalize::expand_maximum(float value) {
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
    } else if (temp > 1.f) {
        return 1.f;
    }

    return temp;
}

float Normalize::linearize(float value) {
    float normalized = normalize(value);

    if (normalized == 0.f) {
        return 1.f;
    }

    float linear = std::sqrt(1 / normalized);

    // Normalize inverse square value to [0..1] given a maximum distance range
    linear = (linear - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE);

    if (linear < 0.f) {
        return 0.f;
    } else if (linear > 1.f) {
        return 1.f;
    }

    return linear;
}

void Normalize::reset() {
    m_min = INFINITY;
    m_max = -INFINITY;
}
