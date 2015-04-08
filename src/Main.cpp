// =============================================================================
// File Name: Main.cpp
// Description: Implements mouse input driver using 3D capacitor
// Author: Tyler Veness
// =============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "SerialPort.hpp"
#include "RenderData.hpp"
#include "Rendering.hpp"

#include "Normalize.hpp"
#include "WeightedAverageFilter.hpp"
#include "KalmanFilter.hpp"
#include "Util.hpp"
#include "Matrix.hpp"

#include <SFML/Window/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

// #define USE_RAW_INPUT

float lastxyz[sen] {0.f, 0.f, 0.f};

// x (left plate), y (bottom plate), z (right plate)
std::vector<Normalize> normalizer(sen);

int main() {
    RenderData renderData;
    const bool flip[sen] = { true, true, true };

#if 0
    // xmin
    normalizer[0].expandRange(17970);

    // xmax
    normalizer[0].expandRange(18150);

    // ymin
    normalizer[1].expandRange(17390);

    // ymax
    normalizer[1].expandRange(17660);

    // zmin
    normalizer[2].expandRange(17415);

    // zmax
    normalizer[2].expandRange(17660);
#endif

    SerialPort serialPort;

#ifdef USE_RAW_INPUT
    float xyz[sen];
#endif

    sf::ContextSettings settings;
    settings.depthBits = 32;
    settings.stencilBits = 0;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    // Setup
    sf::Window mainWin(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Cube",
        sf::Style::Resize | sf::Style::Close, settings);
    mainWin.setFramerateLimit(25);
    sf::Window mainWin2(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Color",
        sf::Style::Resize | sf::Style::Close, settings);
    mainWin2.setFramerateLimit(25);

    mainWin.setActive();

    sf::Vector2i lastMousePos = sf::Mouse::getPosition(mainWin);

    // Set buffer clear values
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearDepth(1.f);

    // Enable depth buffering
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable textures since they are unneeded
    glDisable(GL_TEXTURE_2D);

    // Declare lighting parameters
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    // Set lighting parameters
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    mainWin2.setActive();
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Used to store data read from serialPort port
    std::string serialPortData;
    char curChar = '\0';
    char numRead = 0;

    sf::Event event;
    while (mainWin.isOpen() && mainWin2.isOpen()) {
        while (mainWin.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mainWin.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    mainWin.close();
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
#if 1
                    // Reset filters
                    for (unsigned int i = 0; i < sen; i++) {
                        normalizer[i].reset();
                        renderData.camera[i].reset();
                        renderData.avgPos[i].reset();
                    }
#endif

                    renderData.rotationMat = Mat::createIdentity<GLfloat>(
                        renderData.rotationMat.height(),
                        renderData.rotationMat.width());
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    float x = event.mouseMove.x - lastMousePos.x;
                    float y = event.mouseMove.y - lastMousePos.y;
                    float mag = std::hypot(x, y);
                    float angle = mag / 2;

                    Matrix<GLfloat> temp(4, 4);
                    temp = Mat::createQuaternion(angle, -y, x, 0.f);

                    renderData.rotationMat = temp * renderData.rotationMat;
                }

                lastMousePos.x = event.mouseMove.x;
                lastMousePos.y = event.mouseMove.y;
            }
        }

        while (mainWin2.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mainWin2.close();
            }
        }

        // Attempt a connection
        if (!serialPort.isConnected()) {
            std::vector<std::string> ports = SerialPort::getSerialPorts();
            if (ports.size() > 0) {
                serialPort.connect(ports[0]);
            }
        }

        // Read line of serialPort data
        if (serialPort.isConnected()) {
            while ((numRead =
                        serialPort.read(&curChar, 1)) > 0 && curChar != '\n' &&
                   curChar != '\0') {
                serialPortData += curChar;
            }

            if (numRead == -1) {
                // EOF has been reached (socket disconnected)
                serialPort.disconnect();
            }
            // If curChar == '\n', there is a new line of complete data
            else if (curChar == '\n' && serialPortData.length() != 0) {
                std::cout << "\"" << serialPortData << "\"\n";

                std::vector<std::string> parts = split(serialPortData, " ");

                if (parts.size() == sen) {
#ifndef USE_RAW_INPUT
                    float xyz[sen];
#endif
                    float raw;

                    renderData.haveValidData = true;

                    for (unsigned int i = 0; i < sen; i++) {
                        xyz[i] = std::atof(parts[i].c_str());

                        std::cout << "diff[" << i << "]=" << std::fabs(
                            xyz[i] - lastxyz[i]) << "\n";

                        if (std::fabs(xyz[i] - lastxyz[i]) < 450 ||
                            lastxyz[i] < 15000) {
                            normalizer[i].expandRange(xyz[i]);
                            lastxyz[i] = xyz[i];
                            std::cout << "defining boundaries\n";
                        }

                        raw = normalizer[i].linearize(xyz[i]);

                        // Update camera and position filters
                        if (flip[i]) {
                            // camera[i].update(1 - raw);
                            renderData.avgPos[i].update(1 - raw);
                        }
                        else {
                            // camera[i].update(raw);
                            renderData.avgPos[i].update(raw);
                        }
                    }
                }
                else {
                    renderData.haveValidData = false;
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
                curChar = '\0';
                numRead = 0;
            }
        }

        renderData.isConnected = serialPort.isConnected();

        renderCube(&mainWin, renderData);
        renderColor(&mainWin2, renderData);
    }

    return 0;
}

