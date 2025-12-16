// Copyright (c) Tyler Veness

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include "KalmanFilter.hpp"
#include "Matrix.hpp"
#include "Normalize.hpp"
#include "RenderData.hpp"
#include "Rendering.hpp"
#include "SerialPort.hpp"
#include "Util.hpp"
#include "WeightedAverageFilter.hpp"

float lastPos[sen]{0.f, 0.f, 0.f};
float rawInput[sen]{0.f, 0.f, 0.f};

// x (left plate), y (bottom plate), z (right plate)

/// Implements mouse input driver using 3D capacitor
int main() {
    RenderData renderData;
    std::vector<Normalize> normalizer(sen);
    const bool flip[sen] = {true, true, true};

    SerialPort serialPort;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    // Setup
    sf::RenderWindow mainWin(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Cube",
        sf::Style::Resize | sf::Style::Close, sf::State::Windowed, settings);
    mainWin.setFramerateLimit(25);
    sf::RenderWindow mainWin2(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Color",
        sf::Style::Resize | sf::Style::Close, sf::State::Windowed, settings);
    mainWin2.setFramerateLimit(25);

    if (!mainWin.setActive(true)) {
        return 1;
    }

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
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

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

    if (!mainWin2.setActive(true)) {
        return -1;
    }
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Used to store data read from serialPort port
    std::string serialPortData;
    char curChar = '\0';
    int numRead = 0;

    while (mainWin.isOpen() && mainWin2.isOpen()) {
        while (auto event = mainWin.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mainWin.close();
            } else if (auto key_event = event->getIf<sf::Event::KeyPressed>()) {
                if (key_event->code == sf::Keyboard::Key::Space) {
                    if (renderData.haveValidData) {
                        for (unsigned int i = 0; i < sen; i++) {
                            normalizer[i].setMinimum(rawInput[i]);
                        }
                    }
                } else if (key_event->code == sf::Keyboard::Key::LShift ||
                           key_event->code == sf::Keyboard::Key::RShift) {
                    renderData.useRawInput = !renderData.useRawInput;
                } else if (key_event->code == sf::Keyboard::Key::LControl ||
                           key_event->code == sf::Keyboard::Key::RControl) {
                    for (auto& obj : renderData.camera) {
                        obj.reset();
                    }
                }
            } else if (auto mouse_event =
                           event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse_event->button == sf::Mouse::Button::Right) {
                    // Reset filters
                    for (unsigned int i = 0; i < sen; i++) {
                        normalizer[i].reset();
                        renderData.camera[i].reset();
                        renderData.avgPos[i].reset();
                    }

                    renderData.rotationMat = Mat::createIdentity<GLfloat>(
                        renderData.rotationMat.height(),
                        renderData.rotationMat.width());
                }
            } else if (auto mouse_event =
                           event->getIf<sf::Event::MouseMoved>()) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    float x = mouse_event->position.x - lastMousePos.x;
                    float y = mouse_event->position.y - lastMousePos.y;
                    float mag = std::hypot(x, y);
                    float angle = mag / 2;

                    Matrix<GLfloat> temp(4, 4);
                    temp = Mat::createQuaternion(angle, -y, x, 0.f);

                    renderData.rotationMat = temp * renderData.rotationMat;
                }

                lastMousePos.x = mouse_event->position.x;
                lastMousePos.y = mouse_event->position.y;
            }
        }

        while (auto event = mainWin2.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
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
            while ((numRead = serialPort.read(&curChar, 1)) > 0 &&
                   curChar != '\n' && curChar != '\0') {
                serialPortData += curChar;
            }

            if (numRead == -1) {
                // EOF has been reached (socket disconnected)
                serialPort.disconnect();
            } else if (curChar == '\n' && serialPortData.length() != 0) {
                // If curChar == '\n', there is a new line of complete data
                std::cout << "\"" << serialPortData << "\"\n";

                std::vector<std::string> parts = split(serialPortData, " ");

                if (parts.size() == sen) {
                    renderData.haveValidData = true;

                    for (unsigned int i = 0; i < sen; i++) {
                        rawInput[i] = std::atof(parts[i].c_str());

                        std::cout << "diff[" << i
                                  << "]=" << std::fabs(rawInput[i] - lastPos[i])
                                  << "\n";

                        if (std::fabs(rawInput[i] - lastPos[i]) < 350 ||
                            lastPos[i] < 15000) {
                            normalizer[i].expandMaximum(rawInput[i]);
                            lastPos[i] = rawInput[i];
                        }

                        float raw = normalizer[i].linearize(rawInput[i]);

                        // Update camera and position filters
                        if (flip[i]) {
                            // renderData.camera[i].update(1 - raw);
                            renderData.avgPos[i].update(1 - raw);
                            renderData.rawPos[i] = 1 - raw;
                        } else {
                            // renderData.camera[i].update(raw);
                            renderData.avgPos[i].update(raw);
                            renderData.rawPos[i] = raw;
                        }
                    }
                } else {
                    renderData.haveValidData = false;
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
            }
        }

        renderData.isConnected = serialPort.isConnected();

        renderCube(&mainWin, renderData);
        renderColor(&mainWin2, renderData);
    }

    return 0;
}
