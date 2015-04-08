// =============================================================================
// File Name: FilterBase.hpp
// Description: Provides an interface for filter classes
// Author: Tyler Veness
// =============================================================================

#ifndef FILTER_BASE_HPP
#define FILTER_BASE_HPP

#include <chrono>
using namespace std::chrono_literals;

class FilterBase {
public:
    FilterBase();
    virtual ~FilterBase() = default;

    virtual void update(double input) = 0;
    virtual void reset();

    // Return the filtered value
    double getEstimate();

protected:
    double m_stateEstimate;

    // Holds dt in update()
    std::chrono::duration<double> m_dt;

    // Used to find dt in update()
    std::chrono::time_point<std::chrono::system_clock> m_lastTime;
};

#endif // FILTER_BASE_HPP

