// Copyright (c) Tyler Veness

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

/// Normalizes numbers from within a given range defined by a min and max to
/// [0..1]
class Normalize {
public:
    /// Will replace either m_min or m_max with value if it is outside of the
    /// range defined by m_min and m_max
    void expand_range(float value) {
        if (value < m_min) {
            m_min = value;
        }

        if (value > m_max) {
            m_max = value;
        }
    }

    void set_minimum(float value) { m_min = value; }

    void expand_maximum(float value) {
        if (value > m_max) {
            m_max = value;
        }
    }

    float normalize(float value) {
        if (m_min == m_max || m_min == std::numeric_limits<float>::infinity()) {
            return 0.f;
        }

        return std::clamp((value - m_min) / (m_max - m_min), 0.f, 1.f);
    }

    float linearize(float value) {
        float norm = normalize(value);
        if (norm == 0.f) {
            return 1.f;
        }
        float linear = std::sqrt(1 / norm);

        // Normalize inverse square value to [0..1] given a distance range
        constexpr float MIN = 1.f;
        constexpr float MAX = 5.f;
        return std::clamp((linear - MIN) / (MAX - MIN), 0.f, 1.f);
    }

    void reset() {
        m_min = std::numeric_limits<float>::infinity();
        m_max = -std::numeric_limits<float>::infinity();
    }

private:
    float m_min = std::numeric_limits<float>::infinity();
    float m_max = -std::numeric_limits<float>::infinity();
};
