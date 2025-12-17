// Copyright (c) Tyler Veness

#pragma once

#include <chrono>
#include <cmath>

/// Implements a Kalman filter for single input filtering
class KalmanFilter {
 public:
  KalmanFilter(double Q, double R) : m_Q{Q}, m_R{R} {}

  void update(double input) {
    // Get the current dt since the last call to update()
    m_dt = std::chrono::system_clock::now() - m_last_time;

    // Initialize estimate to measured value
    if (m_first_run) {
      m_x = input;

      m_first_run = false;
    }

    // Project the error covariance ahead
    m_P += m_Q * m_dt.count();

    // Calculate the Kalman gain
    double K = m_P / (m_P + m_R);

    // Correct state estimate
    m_x += K * (input - m_x);
    m_P -= K * m_P;

    if (std::isnan(m_x)) {
      m_x = 0.0;
    }

    // Update the previous time for the next delta
    m_last_time = std::chrono::system_clock::now();
  }

  /// Return the filtered value
  constexpr double get_estimate() { return m_x; }

  void reset() {
    using namespace std::chrono_literals;

    m_x = 0.0;
    m_P = 0.0;

    m_dt = 0s;
    m_last_time = std::chrono::system_clock::now();

    m_first_run = true;
  }

 private:
  // State estimate
  double m_x = 0.0;

  // Covariance of error
  double m_P = 0.0;

  // Process noise variance
  double m_Q;

  // Measurement noise variance
  double m_R;

  /// Holds dt in update()
  std::chrono::duration<double> m_dt{0.0};

  /// Used to find dt in update()
  std::chrono::time_point<std::chrono::system_clock> m_last_time{
      std::chrono::system_clock::now()};

  // If true, update() sets current estimate to first measurement
  bool m_first_run = true;
};
