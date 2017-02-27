// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#pragma once

#include <GL/gl.h>

/* fillType can be either GL_FILL or GL_LINE
 * GL_FILL fills surface with color; GL_LINE draws frame
 */
void drawBox(float width, GLenum fillType);

void drawCircle(float radius, float points);
