// =============================================================================
// File Name: Rendering.hpp
// Description: Provides helper functions for drawing various elements of the
//              GUI with OpenGL
// Author: Tyler Veness
// =============================================================================

#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include "RenderData.hpp"

const unsigned int subDivs = 3; // board sub-divisions
const float w = 256; // board size

void renderConnectionIndicator(sf::RenderWindow* window, RenderData& data);

void renderCube(sf::RenderWindow* window, RenderData& data);

void renderColor(sf::RenderWindow* window, RenderData& data);

#endif // RENDERING_HPP

