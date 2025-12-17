// Copyright (c) Tyler Veness

#pragma once

#include <cstdlib>

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif

#if defined(__APPLE__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/OpenGL.hpp>

#include "gl_utils.hpp"
#include "render_data.hpp"

inline constexpr int BOARD_SUBDIVISIONS = 3;
inline constexpr float BOARD_SIZE = 256;

inline void render_connection_indicator(sf::RenderWindow* window,
                                        RenderData& data) {
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

  if (data.is_connected) {
    if (data.have_valid_data) {
      // Connected and valid data
      glColor3ub(0, 200, 0);
    } else {
      // Connected but no valid data
      glColor3ub(255, 220, 0);
    }
  } else {
    // Disconnected
    glColor3ub(200, 0, 0);
  }

  glDisable(GL_LIGHTING);
  draw_circle(18.f, 32);
  glEnable(GL_LIGHTING);

  // Restore modelview matrix
  glPopMatrix();

  // Restore projection matrix
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

inline void render_cube(sf::RenderWindow* window, RenderData& data) {
  if (!window->setActive(true)) {
    std::exit(1);
  }

  // Set up window
  glViewport(0, 0, window->getSize().x, window->getSize().y);

  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  render_connection_indicator(window, data);

  // Set up window
  // glViewport(0, 0, window->getSize().x, window->getSize().y);

  // Set up projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.f,
                 static_cast<float>(window->getSize().x) / window->getSize().y,
                 200.f, 900.f);

  // Set up modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(
      BOARD_SIZE / 2 +
          (data.camera[0].get_estimate() - data.camera[2].get_estimate()) *
              BOARD_SIZE / 2,
      (BOARD_SIZE * 3 +
       (data.camera[1].get_estimate() - 1) * window->getSize().y / 2),
      BOARD_SIZE * 2, BOARD_SIZE / 2, BOARD_SIZE / 2, BOARD_SIZE / 2, 0, 1, 0);

  /* The sensor's coordinate axes are oriented differently from OpenGL's
   * axes, so rotate the view until they match. glTranslatef() is used to
   * make rotation occur around center of cube. This translation is
   * undone manually since pushing and popping a matrix would undo the
   * rotation as well.
   */
  glTranslatef(BOARD_SIZE / 2, BOARD_SIZE / 2, BOARD_SIZE / 2);
  glRotatef(180.f, 1.f, 0.f, 0.f);

  glMultMatrixf(
      data.rotation_mat.transpose().data());  // Rotate view with mouse
  glTranslatef(-BOARD_SIZE / 2, -BOARD_SIZE / 2, -BOARD_SIZE / 2);

  glPushMatrix();

  // Draw outer boundary box
  glColor4ub(0, 0, 0, 40);
  glTranslatef(BOARD_SIZE / 2, BOARD_SIZE / 2, BOARD_SIZE / 2);
  glRotatef(-45.f, 0.f, 1.f, 0.f);
  draw_box(BOARD_SIZE, GL_LINE);

  glPopMatrix();

  constexpr float SUBDIV_WIDTH = BOARD_SIZE / BOARD_SUBDIVISIONS;

  glTranslatef(BOARD_SIZE / 2, SUBDIV_WIDTH / 2, 0);
  glRotatef(-45.f, 0.f, 1.f, 0.f);

  glPushMatrix();

  /* Converts normalized average to position within cube
   *   axyz * (BOARD_SUBDIVISIONS - 1) * (BOARD_SIZE / BOARD_SUBDIVISIONS)
   * = axyz * (BOARD_SIZE - BOARD_SIZE / BOARD_SUBDIVISIONS)
   * = axyz * (BOARD_SIZE - SUBDIV_WIDTH)
   */
  constexpr float POS_MODIFIER = BOARD_SIZE - SUBDIV_WIDTH;

  if (data.use_raw_input) {
    glTranslatef(data.raw_pos[0] * POS_MODIFIER, data.raw_pos[1] * POS_MODIFIER,
                 data.raw_pos[2] * POS_MODIFIER);
  } else {
    glTranslatef(data.avg_pos[0].get_estimate() * POS_MODIFIER,
                 data.avg_pos[1].get_estimate() * POS_MODIFIER,
                 data.avg_pos[2].get_estimate() * POS_MODIFIER);
  }

  // Draw sphere for current position of hand
  glColor4ub(255, 160, 0, 200);
  GLUquadricObj* sphere = gluNewQuadric();
  if (sphere != nullptr) {
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 18, 32, 32);
    gluDeleteQuadric(sphere);
  }

  glPopMatrix();

  for (int z = 0; z < BOARD_SUBDIVISIONS; ++z) {
    for (int y = 0; y < BOARD_SUBDIVISIONS; ++y) {
      for (int x = 0; x < BOARD_SUBDIVISIONS; ++x) {
        glPushMatrix();

        glTranslatef(x * SUBDIV_WIDTH, y * SUBDIV_WIDTH, z * SUBDIV_WIDTH);

        /* Converts normalized position estimate [0..1] to
         * position in array [0..BOARD_SUBDIVISIONS-1]
         */
        if (x == std::lround(data.avg_pos[0].get_estimate() *
                             (BOARD_SUBDIVISIONS - 1)) &&
            y == std::lround(data.avg_pos[1].get_estimate() *
                             (BOARD_SUBDIVISIONS - 1)) &&
            z == std::lround(data.avg_pos[2].get_estimate() *
                             (BOARD_SUBDIVISIONS - 1))) {
          // transparent red
          glColor4ub(255, 0, 0, 200);
        } else {
          // transparent gray
          glColor4ub(100, 100, 100, 100);
        }
        draw_box(SUBDIV_WIDTH / 3, GL_FILL);

        glPopMatrix();
      }
    }
  }

  window->pushGLStates();

  sf::Text text(data.font, "", 12);
  text.setFillColor(sf::Color::Black);

  text.setString("raw x: " + std::to_string(data.raw_pos[0]));
  text.setPosition({18, 68});
  window->draw(text);

  text.setString("raw y: " + std::to_string(data.raw_pos[1]));
  text.setPosition({18, text.getPosition().y + 12});
  window->draw(text);

  text.setString("raw z: " + std::to_string(data.raw_pos[2]));
  text.setPosition({18, text.getPosition().y + 12});
  window->draw(text);

  text.setString("avg x: " + std::to_string(data.avg_pos[0].get_estimate()));
  text.setPosition({18, text.getPosition().y + 24});
  window->draw(text);

  text.setString("avg y: " + std::to_string(data.avg_pos[1].get_estimate()));
  text.setPosition({18, text.getPosition().y + 12});
  window->draw(text);

  text.setString("avg z: " + std::to_string(data.avg_pos[2].get_estimate()));
  text.setPosition({18, text.getPosition().y + 12});
  window->draw(text);

  window->popGLStates();

  window->display();
}

inline void render_color(sf::RenderWindow* window, RenderData& data) {
  if (!window->setActive(true)) {
    std::exit(1);
  }

  // Set up window
  glViewport(0, 0, window->getSize().x, window->getSize().y);

  if (data.use_raw_input) {
    glClearColor(data.raw_pos[0], data.raw_pos[1], data.raw_pos[2], 1.f);
  } else {
    glClearColor(data.avg_pos[0].get_estimate(), data.avg_pos[1].get_estimate(),
                 data.avg_pos[2].get_estimate(), 1.f);
  }
  glClear(GL_COLOR_BUFFER_BIT);

  render_connection_indicator(window, data);

  window->display();
}
