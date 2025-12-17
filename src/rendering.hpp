// Copyright (c) Tyler Veness

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "render_data.hpp"

inline constexpr int BOARD_SUBDIVISIONS = 3;
inline constexpr float BOARD_SIZE = 256;

void render_connection_indicator(sf::RenderWindow* window, RenderData& data);

void render_cube(sf::RenderWindow* window, RenderData& data);

void render_color(sf::RenderWindow* window, RenderData& data);
