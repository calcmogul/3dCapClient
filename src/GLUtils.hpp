// Copyright (c) Tyler Veness

#pragma once

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif
#include <SFML/OpenGL.hpp>

/// fillType can be either GL_FILL or GL_LINE.
/// GL_FILL fills surface with color; GL_LINE draws frame.
void drawBox(float width, GLenum fillType);

void drawCircle(float radius, int points);
