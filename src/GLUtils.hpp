// =============================================================================
// File Name: GLUtils.hpp
// Description: Provides convenience functions for drawing OpenGL shapes
// Author: Tyler Veness
// =============================================================================

#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP

#include <GL/gl.h>

/* fillType can be either GL_FILL or GL_LINE
 * GL_FILL fills surface with color; GL_LINE draws frame
 */
void drawBox(float width, GLenum fillType);

void drawCircle(float radius, float points);

#endif // GL_UTILS_HPP

