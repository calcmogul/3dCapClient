// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include <cmath>

/**
 * Normalizes numbers from within a given range defined by a min and max to
 * [0..1]
 */
class Normalize {
public:
    /* Will replace either m_min or m_max with value if it is outside of the
     * range defined by m_min and m_max
     */
    void expandRange(float value);

    void setMinimum(float value);
    void expandMaximum(float value);

    float normalize(float value);
    float linearize(float value);
    void reset();

private:
    float m_min{INFINITY};
    float m_max{-INFINITY};

    static constexpr float m_minDistance{1.f};
    static constexpr float m_maxDistance{5.f};
};
