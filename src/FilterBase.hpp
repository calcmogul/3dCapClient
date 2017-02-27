// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include <chrono>

/**
 * Provides an interface for filter classes
 */
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
