// =============================================================================
// File Name: GLUtils.cpp
// Description: Provides convenience functions for drawing OpenGL shapes
// Author: Tyler Veness
// =============================================================================

#include "GLUtils.hpp"
#include <GL/glu.h>
#include <cmath>

void drawBox(float width, GLenum fillType) {
    float height = width, depth = width;

    glPushMatrix();

    // Add equally to x and z to move up and down in finished view
    glTranslatef(-width / 2, -height / 2, depth / 2);

    if (fillType == GL_FILL) {
        glBegin(GL_TRIANGLE_STRIP);
        // Front right vertical line
        glVertex3f(width, 0, 0);
        glVertex3f(width, height, 0);

        // Rear right vertical line
        glVertex3f(width, 0, -depth);
        glVertex3f(width, height, -depth);

        // Rear left vertical line
        glVertex3f(0, 0, -depth);
        glVertex3f(0, height, -depth);

        // Front left vertical line
        glVertex3f(0, 0, 0);
        glVertex3f(0, height, 0);

        // Front right vertical line
        glVertex3f(width, 0, 0);
        glVertex3f(width, height, 0);
        glEnd();

        glBegin(GL_TRIANGLE_STRIP);
        // Top right horizontal line
        glVertex3f(width, height, 0);
        glVertex3f(width, height, -depth);

        // Top left horizontal line
        glVertex3f(0, height, 0);
        glVertex3f(0, height, -depth);
        glEnd();

        glBegin(GL_TRIANGLE_STRIP);
        // Bottom right horizontal line
        glVertex3f(width, 0, 0);
        glVertex3f(width, 0, -depth);

        // Bottom left horizontal line
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -depth);
        glEnd();
    }
    else if (fillType == GL_LINE) {
        // Front face
        glBegin(GL_LINE_STRIP);
        glVertex3f(width, 0, 0);
        glVertex3f(width, height, 0);
        glVertex3f(0, height, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(width, 0, 0);
        glEnd();

        // Right face
        glBegin(GL_LINE_STRIP);
        glVertex3f(width, 0, 0);
        glVertex3f(width, 0, -depth);
        glVertex3f(width, height, -depth);
        glVertex3f(width, height, 0);
        glEnd();

        // Rear face
        glBegin(GL_LINE_STRIP);
        glVertex3f(width, 0, -depth);
        glVertex3f(0, 0, -depth);
        glVertex3f(0, height, -depth);
        glVertex3f(width, height, -depth);
        glEnd();

        // Left face
        glBegin(GL_LINE_STRIP);
        glVertex3f(0, 0, -depth);
        glVertex3f(0, 0, 0);
        glVertex3f(0, height, 0);
        glVertex3f(0, height, -depth);
        glEnd();
    }

    glPopMatrix();
}

void drawCircle(float radius, float points) {
    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.f, 0.f, 0.f);

    for (float theta = 0.f; theta < 360.f; theta += 360.f / points) {
        glVertex3f(
            radius * cos(theta * M_PI / 180.f),
            radius * sin(theta * M_PI / 180.f),
            0.f
            );
    }

    glVertex3f(
        radius,
        0.f,
        0.f
        );

    glEnd();
}

