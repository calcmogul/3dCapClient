// Copyright (c) Tyler Veness

#include <array>
#include <cmath>
#include <numbers>
#include <print>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <SFML/Graphics/RenderWindow.hpp>
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include "normalize.hpp"
#include "render_data.hpp"
#include "rendering.hpp"
#include "serial_port.hpp"
#include "util.hpp"
#include "weighted_average_filter.hpp"

float last_pos[SENSORS]{0.f, 0.f, 0.f};
float raw_input[SENSORS]{0.f, 0.f, 0.f};

/// Embeds a quaternion in a 4x4 matrix.
///
/// @param angle Rotation around axis in radians.
/// @param x x component of rotation axis.
/// @param y y component of rotation axis.
/// @param z z component of rotation axis.
Eigen::Matrix4f make_quaternion(float angle, float x, float y, float z) {
    Eigen::Matrix4f mat;

    float mag = std::hypot(x, y, z);
    float c = std::cos(angle);
    float s = std::sin(angle);

    if (mag != 0) {
        x /= mag;
        y /= mag;
        z /= mag;
    }

    mat(0, 0) = x * x * (1 - c) + c;
    mat(0, 1) = y * x * (1 - c) + z * s;
    mat(0, 2) = x * z * (1 - c) - y * s;
    mat(0, 3) = 0;
    mat(1, 0) = x * y * (1 - c) - z * s;
    mat(1, 1) = y * y * (1 - c) + c;
    mat(1, 2) = y * z * (1 - c) + x * s;
    mat(1, 3) = 0;
    mat(2, 0) = x * z * (1 - c) + y * s;
    mat(2, 1) = y * z * (1 - c) - x * s;
    mat(2, 2) = z * z * (1 - c) + c;
    mat(2, 3) = 0;
    mat(3, 0) = 0;
    mat(3, 1) = 0;
    mat(3, 2) = 0;
    mat(3, 3) = 1;

    return mat;
}

// x (left plate), y (bottom plate), z (right plate)

/// Implements mouse input driver using 3D capacitor
int main() {
    RenderData render_data;
    std::array<Normalize, SENSORS> normalizer;
    constexpr std::array<bool, SENSORS> FLIP{true, true, true};

    SerialPort serial_port;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    // Setup
    sf::RenderWindow main_win(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Cube",
        sf::Style::Resize | sf::Style::Close, sf::State::Windowed, settings);
    main_win.setFramerateLimit(25);
    sf::RenderWindow main_win2(
        sf::VideoMode::getDesktopMode(), "3D Capacitor Demo - Color",
        sf::Style::Resize | sf::Style::Close, sf::State::Windowed, settings);
    main_win2.setFramerateLimit(25);

    if (!main_win.setActive(true)) {
        return 1;
    }

    sf::Vector2i lastMousePos = sf::Mouse::getPosition(main_win);

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

    if (!main_win2.setActive(true)) {
        return -1;
    }
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Used to store data read from serial_port port
    std::string serial_port_data;

    while (main_win.isOpen() && main_win2.isOpen()) {
        while (auto event = main_win.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                main_win.close();
            } else if (auto key_event = event->getIf<sf::Event::KeyPressed>()) {
                if (key_event->code == sf::Keyboard::Key::Space) {
                    if (render_data.have_valid_data) {
                        for (unsigned int i = 0; i < SENSORS; i++) {
                            normalizer[i].set_minimum(raw_input[i]);
                        }
                    }
                } else if (key_event->code == sf::Keyboard::Key::LShift ||
                           key_event->code == sf::Keyboard::Key::RShift) {
                    render_data.use_raw_input = !render_data.use_raw_input;
                } else if (key_event->code == sf::Keyboard::Key::LControl ||
                           key_event->code == sf::Keyboard::Key::RControl) {
                    for (auto& obj : render_data.camera) {
                        obj.reset();
                    }
                }
            } else if (auto mouse_event =
                           event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse_event->button == sf::Mouse::Button::Right) {
                    // Reset filters
                    for (unsigned int i = 0; i < SENSORS; i++) {
                        normalizer[i].reset();
                        render_data.camera[i].reset();
                        render_data.avg_pos[i].reset();
                    }

                    render_data.rotation_mat = Eigen::Matrix4f::Identity();
                }
            } else if (auto mouse_event =
                           event->getIf<sf::Event::MouseMoved>()) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    float x = mouse_event->position.x - lastMousePos.x;
                    float y = mouse_event->position.y - lastMousePos.y;
                    float mag = std::hypot(x, y);
                    float angle = mag / 2;

                    Eigen::Matrix4f temp;
                    temp = make_quaternion(angle * std::numbers::pi / 180.f, -y,
                                           x, 0.f);

                    render_data.rotation_mat = temp * render_data.rotation_mat;
                }

                lastMousePos.x = mouse_event->position.x;
                lastMousePos.y = mouse_event->position.y;
            }
        }

        while (auto event = main_win2.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                main_win2.close();
            }
        }

        // Attempt a connection
        if (!serial_port.is_connected()) {
            std::vector<std::string> ports = SerialPort::get_serial_ports();
            if (ports.size() > 0) {
                serial_port.connect(ports[0]);
            }
        }

        // Read line of serial_port data
        if (serial_port.is_connected()) {
            char curChar;
            int numRead;
            while ((numRead = serial_port.read(&curChar, 1)) > 0 &&
                   curChar != '\n' && curChar != '\0') {
                serial_port_data += curChar;
            }

            if (numRead == -1) {
                // EOF has been reached (socket disconnected)
                serial_port.disconnect();
            } else if (curChar == '\n' && serial_port_data.length() != 0) {
                // If curChar == '\n', there is a new line of complete data
                std::println("\"{}\"", serial_port_data);

                std::vector<std::string> parts = split(serial_port_data, " ");

                if (parts.size() == SENSORS) {
                    render_data.have_valid_data = true;

                    for (unsigned int i = 0; i < SENSORS; i++) {
                        raw_input[i] = std::atof(parts[i].c_str());

                        std::println("diff[{}]={}", i,
                                     std::fabs(raw_input[i] - last_pos[i]));

                        if (std::fabs(raw_input[i] - last_pos[i]) < 350 ||
                            last_pos[i] < 15000) {
                            normalizer[i].expand_maximum(raw_input[i]);
                            last_pos[i] = raw_input[i];
                        }

                        float raw = normalizer[i].linearize(raw_input[i]);

                        // Update camera and position filters
                        if (FLIP[i]) {
                            // render_data.camera[i].update(1 - raw);
                            render_data.avg_pos[i].update(1 - raw);
                            render_data.raw_pos[i] = 1 - raw;
                        } else {
                            // render_data.camera[i].update(raw);
                            render_data.avg_pos[i].update(raw);
                            render_data.raw_pos[i] = raw;
                        }
                    }
                } else {
                    render_data.have_valid_data = false;
                }

                // Reset serial data storage in preparation for new line of data
                serial_port_data.clear();
            }
        }

        render_data.is_connected = serial_port.is_connected();

        render_cube(&main_win, render_data);
        render_color(&main_win2, render_data);
    }

    return 0;
}
