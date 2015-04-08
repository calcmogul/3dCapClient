// =============================================================================
// File Name: WeightedAverageFilter.hpp
// Description: Averages new value with old one using a given weight for the new
//             one [0..1]
// Author: Tyler Veness
// =============================================================================

#ifndef WEIGHTED_AVERAGE_FILTER_HPP
#define WEIGHTED_AVERAGE_FILTER_HPP

#include "FilterBase.hpp"

// m_stateEstimate == the average state

class WeightedAverageFilter : public FilterBase {
public:
    explicit WeightedAverageFilter(float adapt);

    void update(double input);

private:
    float m_adapt;
};

#endif // WEIGHTED_AVERAGE_FILTER_HPP

