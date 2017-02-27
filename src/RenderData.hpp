// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/OpenGL.hpp>

#include "KalmanFilter.hpp"
#include "Matrix.hpp"
#include "WeightedAverageFilter.hpp"

const unsigned int sen = 3;  // sensors

/**
 * A container for globals to pass to rendering functions
 */
class RenderData {
public:
    RenderData();

    std::vector<KalmanFilter> avgPos{sen, KalmanFilter(0.00004, 0.0004)};
    std::vector<WeightedAverageFilter> camera{sen, WeightedAverageFilter(0.04)};
    bool isConnected{false};
    bool haveValidData{false};
    Matrix<GLfloat> rotationMat{4, 4, true};

    std::vector<float> rawPos{sen, 0.f};
    bool useRawInput{false};

    sf::Font font;
};
