// Copyright (c) Tyler Veness

#pragma once

#include <cmath>

/// Normalizes numbers from within a given range defined by a min and max to
/// [0..1]
class Normalize {
public:
    /// Will replace either m_min or m_max with value if it is outside of the
    /// range defined by m_min and m_max
    void expand_range(float value);

    void set_minimum(float value);
    void expand_maximum(float value);

    float normalize(float value);
    float linearize(float value);
    void reset();

private:
    float m_min{INFINITY};
    float m_max{-INFINITY};

    static constexpr float MIN_DISTANCE = 1.f;
    static constexpr float MAX_DISTANCE = 5.f;
};
