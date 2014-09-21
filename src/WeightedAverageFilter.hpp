//=============================================================================
//File Name: WeightedAverageFilter.hpp
//Description: Averages new value with old one using a given weight for the new
//             one [0..1]
//Author: Tyler Veness
//=============================================================================

#ifndef WEIGHTED_AVERAGE_FILTER_HPP
#define WEIGHTED_AVERAGE_FILTER_HPP

class WeightedAverageFilter {
public:
    WeightedAverageFilter( float adapt );

    void update( float input );
    void reset();

    // Return the filtered value
    float getEstimate();

private:
    float m_adapt;
    float m_avg;
};

#endif // WEIGHTED_AVERAGE_FILTER_HPP
