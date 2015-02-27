// =============================================================================
// File Name: Normalize.hpp
// Description: Normalizes numbers from within a given range defined by a min
//             and max to [0..1]
// Author: Tyler Veness
// =============================================================================

#ifndef NORMALIZE_HPP
#define NORMALIZE_HPP

class Normalize {
public:
    Normalize();

    /* Will replace either m_min or m_max with value if it is outside of the
     * range defined by m_min and m_max
     */
    void expandRange(float value);

    float normalize(float value);
    float linearize(float value);
    void reset();

private:
    float m_min;
    float m_max;

    static const bool m_isLinear;
    static const float m_minDistance;
    static const float m_maxDistance;
};

#endif // NORMALIZE_HPP

