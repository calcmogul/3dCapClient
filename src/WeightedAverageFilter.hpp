//=============================================================================
//File Name: WeightedAverageFilter.hpp
//Description: Averages new value with old one using a given weight for the new
//             one [0..1]
//Author: Tyler Veness
//=============================================================================

#ifndef WEIGHTED_AVERAGE_FILTER_HPP
#define WEIGHTED_AVERAGE_FILTER_HPP

#include <SFML/System/Clock.hpp>

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

    // Holds dt in update()
    double m_dt;

    // Used to find dt in update()
    sf::Clock m_time;
};

#endif // WEIGHTED_AVERAGE_FILTER_HPP
