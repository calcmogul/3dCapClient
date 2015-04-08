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

#include "Normalize.hpp"
#include "WeightedAverageFilter.hpp"
#include "KalmanFilter.hpp"
#include "Util.hpp"
#include "GLUtils.hpp"
#include "Matrix.hpp"

#include <SFML/Window/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

// #define USE_RAW_INPUT

const unsigned int sen = 3; // sensors
const unsigned int subDivs = 3; // board sub-divisions

const float w = 256; // board size

bool haveValidData{false};
bool calibrate{false};

// x (left plate), y (bottom plate), z (right plate)
std::vector<Normalize> n(sen);
std::vector<WeightedAverageFilter> cama(sen, WeightedAverageFilter(0.04));
std::vector<KalmanFilter> axyz(sen, KalmanFilter(0.00004, 0.0001));
unsigned char ixyz[sen];
Matrix<GLfloat> rotationMat(4, 4, true);

// 'isConnected' referrs to whether client's serial port is connected
void renderCube(sf::Window* window, bool isConnected) {
    window->setActive();

    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // Draw connection indicator
    glPushMatrix();

    // Set up screen
    glViewport(0, 0, window->getSize().x, window->getSize().y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window->getSize().x, window->getSize().y, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Translate circle away from top left of window
    glTranslatef(36.f, 36.f, 0.f);

    if (isConnected) {
        if (haveValidData) {
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

    gluLookAt(
            0.f, 0.f, w * 2,
            0.f, 0.f, w / 2,
            0, 1, 0);

    glPopMatrix();

    // Set up screen
    glViewport(0, 0, window->getSize().x, window->getSize().y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f,
                   static_cast<float>(window->getSize().x) / window->getSize().y,
                   250.f,
                   900.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        w / 2 + (cama[0].getEstimate() - cama[2].getEstimate()) * w / 2,
        (w * 3 + (cama[1].getEstimate() - 1) * window->getSize().y / 2),
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

    glMultMatrixf(rotationMat.transpose().data()); // Rotate view with mouse
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
        axyz[0].getEstimate() * posModifier,
        axyz[1].getEstimate() * posModifier,
        axyz[2].getEstimate() * posModifier);
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

                if (x == ixyz[0] &&
                    y == ixyz[1] &&
                    z == ixyz[2]) {
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

    glPopMatrix();

    window->display();
}

void renderColor(sf::Window* window) {
    window->setActive();
    glClearColor(axyz[0].getEstimate(),
                 axyz[1].getEstimate(),
                 axyz[2].getEstimate(),
                 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window->display();
}

int main() {
    const bool flip[sen] = { true, true, true };

    // xmin
    n[0].expandRange(17970);

    // xmax
    n[0].expandRange(18150);

    // ymin
    n[1].expandRange(17390);

    // ymax
    n[1].expandRange(17660);

    // zmin
    n[2].expandRange(17415);

    // zmax
    n[2].expandRange(17660);

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
    sf::Window mainWin(sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Cube",
                       sf::Style::Resize | sf::Style::Close, settings);
    mainWin.setFramerateLimit(25);
    sf::Window mainWin2(sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Color",
                        sf::Style::Resize | sf::Style::Close, settings);
    mainWin2.setFramerateLimit(25);

    mainWin.setActive();

    sf::Vector2i lastMousePos = sf::Mouse::getPosition(mainWin);

    // Set buffer clear values
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    // Set up screen
    glViewport(0, 0, mainWin.getSize().x, mainWin.getSize().y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f,
                   static_cast<float>(mainWin.getSize().x) / mainWin.getSize().y,
                   250.f,
                   900.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    mainWin2.setActive();
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, mainWin2.getSize().x, mainWin2.getSize().y);

    mainWin.setActive(false);
    mainWin2.setActive(false);

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
                if (event.key.code == sf::Keyboard::Space) {
                    calibrate = true;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    mainWin.close();
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Space) {
                    calibrate = false;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    // Reset filters
                    for (unsigned int i = 0; i < sen; i++) {
                        n[i].reset();
                        cama[i].reset();
                        axyz[i].reset();
                    }

                    rotationMat = Mat::createIdentity<GLfloat>(
                        rotationMat.height(),
                        rotationMat.width());
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

                    rotationMat = temp * rotationMat;
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

                    haveValidData = true;

                    for (unsigned int i = 0; i < sen; i++) {
                        xyz[i] = std::atof(parts[i].c_str());

                        if (calibrate) {
                            n[i].expandRange(xyz[i]);
                            std::cout << "defining boundaries\n";
                        }

                        raw = n[i].linearize(xyz[i]);

                        // Update camera and position filters
                        if (flip[i]) {
                            // cama[i].update(1 - raw);
                            axyz[i].update(1 - raw);
                        }
                        else {
                            // cama[i].update(raw);
                            axyz[i].update(raw);
                        }

                        /* Converts normalized position estimate [0..1] to
                         * position in array [0..subDivs-1]
                         */
                        ixyz[i] =
                            std::lround(axyz[i].getEstimate() * (subDivs - 1));

                        serialPort.write(reinterpret_cast<char*>(&(*ixyz)),
                                         sizeof(ixyz));
                    }
                }
                else {
                    haveValidData = false;
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
                curChar = '\0';
                numRead = 0;
            }
        }

        renderCube(&mainWin, serialPort.isConnected());
        renderColor(&mainWin2);
    }

    return 0;
}

