// Copyright (c) Tyler Veness

#pragma once

#include <chrono>

/// Provides an interface for filter classes
class FilterBase {
public:
    FilterBase() = default;
    virtual ~FilterBase() = default;

    virtual void update(double input) = 0;

    void reset() {
        using namespace std::chrono_literals;

        m_state_estimate = 0.0;
        m_dt = 0.0s;
        m_last_time = std::chrono::system_clock::now();
    }

    /// Return the filtered value
    double get_estimate() { return m_state_estimate; }

protected:
    double m_state_estimate = 0.0;

    /// Holds dt in update()
    std::chrono::duration<double> m_dt{0.0};

    /// Used to find dt in update()
    std::chrono::time_point<std::chrono::system_clock> m_last_time{
        std::chrono::system_clock::now()};
};
