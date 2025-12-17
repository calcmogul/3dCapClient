// Copyright (c) Tyler Veness

#pragma once

#include <array>

#include <Eigen/Core>
#include <SFML/Graphics/Font.hpp>
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif
#include <SFML/OpenGL.hpp>

#include "kalman_filter.hpp"
#include "weighted_average_filter.hpp"

inline constexpr int SENSORS = 3;

/// A container for globals to pass to rendering functions
class RenderData {
public:
    std::array<KalmanFilter, SENSORS> avg_pos{KalmanFilter{4e-5, 4e-4},
                                              KalmanFilter{4e-5, 4e-4},
                                              KalmanFilter{4e-5, 4e-4}};
    std::array<WeightedAverageFilter, SENSORS> camera{
        WeightedAverageFilter{4e-2}, WeightedAverageFilter{4e-2},
        WeightedAverageFilter{4e-2}};
    bool is_connected{false};
    bool have_valid_data{false};
    Eigen::Matrix4f rotation_mat = Eigen::Matrix4f::Identity();

    std::array<float, SENSORS> raw_pos{0.f, 0.f, 0.f};
    bool use_raw_input{false};

    sf::Font font{"arial.ttf"};
};
