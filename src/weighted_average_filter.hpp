// Copyright (c) Tyler Veness

#pragma once

#include <chrono>
#include <cmath>

/// Averages new value with old one using a given weight for the new one [0..1]
class WeightedAverageFilter {
 public:
  explicit WeightedAverageFilter(float alpha) : m_alpha{alpha} {}

  void update(double input) {
    if (std::isnan(input)) {
      return;
    }

    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_last_time;

    m_x = m_x * (1 - m_alpha * m_dt.count()) + input * m_alpha * m_dt.count();

    // Update the previous time for the next delta
    m_last_time = std::chrono::system_clock::now();
  }

  /// Return the filtered value
  constexpr double get_estimate() { return m_x; }

  void reset() {
    using namespace std::chrono_literals;

    m_x = 0.0;

    m_dt = 0s;
    m_last_time = std::chrono::system_clock::now();
  }

 private:
  // State estimate
  double m_x = 0.0;

  float m_alpha;

  /// Holds dt in update()
  std::chrono::duration<double> m_dt{0.0};

  /// Used to find dt in update()
  std::chrono::time_point<std::chrono::system_clock> m_last_time{
      std::chrono::system_clock::now()};
};
