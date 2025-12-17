// Copyright (c) Tyler Veness

#pragma once

#include "filter_base.hpp"

// m_stateEstimate == the average state

/// Averages new value with old one using a given weight for the new one [0..1]
class WeightedAverageFilter : public FilterBase {
public:
    explicit WeightedAverageFilter(float adapt);

    void update(double input);

private:
    float m_adapt;
};
