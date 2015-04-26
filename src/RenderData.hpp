// =============================================================================
// File Name: RenderData.hpp
// Description: A container for globals to pass to rendering functions
// Author: Tyler Veness
// =============================================================================

#ifndef RENDER_DATA_HPP
#define RENDER_DATA_HPP

#include <vector>
#include "KalmanFilter.hpp"
#include "WeightedAverageFilter.hpp"
#include "Matrix.hpp"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Font.hpp>

const unsigned int sen = 3; // sensors

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

#endif // RENDER_DATA_HPP

