// Copyright (c) Tyler Veness

#pragma once

#include <vector>

#include <SFML/Graphics/Font.hpp>
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif
#include <SFML/OpenGL.hpp>

#include "kalman_filter.hpp"
#include "matrix.hpp"
#include "weighted_average_filter.hpp"

inline constexpr unsigned int SENSORS = 3;

/// A container for globals to pass to rendering functions
class RenderData {
public:
    std::vector<KalmanFilter> avg_pos{SENSORS, KalmanFilter(0.00004, 0.0004)};
    std::vector<WeightedAverageFilter> camera{SENSORS,
                                              WeightedAverageFilter(0.04)};
    bool is_connected{false};
    bool have_valid_data{false};
    Matrix<GLfloat> rotation_mat{4, 4, true};

    std::vector<float> raw_pos{SENSORS, 0.f};
    bool use_raw_input{false};

    sf::Font font{"arial.ttf"};
};
