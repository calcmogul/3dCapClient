// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include "RenderData.hpp"

const unsigned int subDivs = 3;  // board sub-divisions
const float w = 256;             // board size

void renderConnectionIndicator(sf::RenderWindow* window, RenderData& data);

void renderCube(sf::RenderWindow* window, RenderData& data);

void renderColor(sf::RenderWindow* window, RenderData& data);
