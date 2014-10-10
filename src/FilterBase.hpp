//=============================================================================
//File Name: FilterBase.hpp
//Description: Provides an interface for filter classes
//Author: Tyler Veness
//=============================================================================

#ifndef FILTER_BASE_HPP
#define FILTER_BASE_HPP

#include <SFML/System/Clock.hpp>

class FilterBase {
public:
    FilterBase();
    virtual ~FilterBase();

    virtual void update( double input ) = 0;
    virtual void reset();

    // Return the filtered value
    double getEstimate();

protected:
    double m_stateEstimate;

    // Holds dt in update()
    double m_dt;

    // Used to find dt in update()
    sf::Clock m_time;
};

#endif // FILTER_BASE_HPP
