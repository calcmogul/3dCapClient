// =============================================================================
// File Name: Rendering.cpp
// Description: Provides helper functions for drawing various elements of the
//              GUI with OpenGL
// Author: Tyler Veness
// =============================================================================

#include "Rendering.hpp"
#include "GLUtils.hpp"

#include <SFML/OpenGL.hpp>

void renderConnectionIndicator(sf::Window* window, RenderData& data) {
    // Save projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    // Set up projection matrix
    glLoadIdentity();
    glOrtho(0, window->getSize().x, window->getSize().y, 0, 0, 1);

    // Save modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Set up modelview matrix
    glLoadIdentity();

    // Translate circle away from top left of window
    glTranslatef(36.f, 36.f, 0.f);

    if (data.isConnected) {
        if (data.haveValidData) {
            // Connected and valid data
            glColor3ub(0, 200, 0);
        }
        else {
            // Connected but no valid data
            glColor3ub(255, 220, 0);
        }
    }
    else {
        // Disconnected
        glColor3ub(200, 0, 0);
    }

    glDisable(GL_LIGHTING);
    drawCircle(18.f, 32.f);
    glEnable(GL_LIGHTING);

    // Restore modelview matrix
    glPopMatrix();

    // Restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void renderCube(sf::Window* window, RenderData& data) {
    window->setActive();

    // Set up window
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderConnectionIndicator(window, data);

    // Set up window
    // glViewport(0, 0, window->getSize().x, window->getSize().y);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f,
                   static_cast<float>(window->getSize().x) / window->getSize().y,
                   200.f,
                   900.f);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        w / 2 + (data.camera[0].getEstimate() - data.camera[2].getEstimate()) * w / 2,
        (w * 3 + (data.camera[1].getEstimate() - 1) * window->getSize().y / 2),
        w * 2,
        w / 2, w / 2, w / 2,
        0, 1, 0);

    /* The sensor's coordinate axes are oriented differently from OpenGL's
     * axes, so rotate the view until they match. glTranslatef() is used to
     * make rotation occur around center of cube. This translation is
     * undone manually since pushing and popping a matrix would undo the
     * rotation as well.
     */
    glTranslatef(w / 2, w / 2, w / 2);
    glRotatef(180.f, 1.f, 0.f, 0.f);

    glMultMatrixf(data.rotationMat.transpose().data()); // Rotate view with mouse
    glTranslatef(-w / 2, -w / 2, -w / 2);

    glPushMatrix();

    // Draw outer boundary box
    glColor4ub(0, 0, 0, 40);
    glTranslatef(w / 2, w / 2, w / 2);
    glRotatef(-45.f, 0.f, 1.f, 0.f);
    drawBox(w, GL_LINE);

    glPopMatrix();

    float subDivWidth = w / subDivs;

    glTranslatef(w / 2, subDivWidth / 2, 0);
    glRotatef(-45.f, 0.f, 1.f, 0.f);

    glPushMatrix();

    /* Converts normalized average to position within cube
     *   axyz * (subDivs - 1) * (w / subDivs)
     * = axyz * (w - w / subDivs)
     * = axyz * ( w - subDivWidth)
     */
    float posModifier = w - subDivWidth;

#ifdef USE_RAW_INPUT
    float raw[sen];
    for (unsigned int i = 0; i < sen; i++) {
        raw[i] = n[i].linearize(xyz[i]);

        if (flip[i]) {
            raw[i] = 1 - raw[i];
        }
    }

    glTranslatef(
        raw[0] * posModifier,
        raw[1] * posModifier,
        raw[2] * posModifier);
#else
    glTranslatef(
        data.avgPos[0].getEstimate() * posModifier,
        data.avgPos[1].getEstimate() * posModifier,
        data.avgPos[2].getEstimate() * posModifier);
#endif

    // Draw sphere for current position of hand
    glColor4ub(255, 160, 0, 200);
    GLUquadricObj* sphere = gluNewQuadric();
    if (sphere != nullptr) {
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluSphere(sphere, 18, 32, 32);
        gluDeleteQuadric(sphere);
    }

    glPopMatrix();

    for (unsigned int z = 0; z < subDivs; z++) {
        for (unsigned int y = 0; y < subDivs; y++) {
            for (unsigned int x = 0; x < subDivs; x++) {
                glPushMatrix();

                glTranslatef(x * subDivWidth,
                             y * subDivWidth,
                             z * subDivWidth);

                /* Converts normalized position estimate [0..1] to
                 * position in array [0..subDivs-1]
                 */
                if (x ==
                    std::lround(data.avgPos[0].getEstimate() * (subDivs - 1)) &&
                    y ==
                    std::lround(data.avgPos[1].getEstimate() * (subDivs - 1)) &&
                    z ==
                    std::lround(data.avgPos[2].getEstimate() * (subDivs - 1))) {
                    // transparent red
                    glColor4ub(255, 0, 0, 200);
                }
                else {
                    // transparent gray
                    glColor4ub(100, 100, 100, 100);
                }
                drawBox(subDivWidth / 3, GL_FILL);

                glPopMatrix();
            }
        }
    }

    window->display();
}

// 'isConnected' refers to whether client's serial port is connected
void renderColor(sf::Window* window, RenderData& data) {
    window->setActive();

    // Set up window
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    glClearColor(data.avgPos[0].getEstimate(),
                 data.avgPos[1].getEstimate(),
                 data.avgPos[2].getEstimate(),
                 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderConnectionIndicator(window, data);

    window->display();
}

