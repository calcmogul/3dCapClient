//=============================================================================
//File Name: Main.cpp
//Description: Implements mouse input driver using 3D capacitor
//Author: Tyler Veness
//=============================================================================

#include <iostream>
#include <string>

#ifdef _WIN32
#include "SerialPort.hpp"
#else
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#endif

#include "Normalize.hpp"
#include "WeightedAverageFilter.hpp"
#include "KalmanFilter.hpp"
#include "Util.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#ifndef _WIN32
// portLabel example: S0, S1, ACM0
int open_port( const char* portLabel );
void set_baud_rate( int fd , speed_t speed );
#endif
void msg( std::string msg );

const unsigned int sen = 3; // sensors
const unsigned int subDivs = 3; // board sub-subDivsisions

Normalize n[sen];
WeightedAverageFilter* cama[sen];
KalmanFilter* axyz[sen];
bool moves[2 * subDivs * subDivs * subDivs];

void resetVariables() {
    for ( unsigned int i = 0 ; i < 2 * subDivs * subDivs * subDivs ; i++ ) {
        moves[i] = false;
    }

    for( unsigned int i = 0 ; i < sen ; i++ ) {
        n[i].reset();
        cama[i]->reset();
        axyz[i]->reset();
    }
}

float cutoff = 0.2;
int getPosition( float x ) {
    if ( subDivs == 3 ) {
        if ( x < cutoff ) {
            return 0;
        }
        else if ( x < 1 - cutoff ) {
            return 1;
        }
        else {
            return 2;
        }
    }
    else {
        return x == 1 ? subDivs - 1 : (int) x * subDivs;
    }
}

int main() {
    /* The argument to open_port should be the serial port of your Arduino.
     *
     * i.e. in Windows, if you have 3 ports: COM1, COM2, COM3 and your Arduino
     * is on COM2, pass "COM2" in.
     *
     * i.e. in Linux, if you have 3 ports: /dev/ttyS0, /dev/ttyS1, /dev/tty2
     * and your Arduino is on ttyS1, pass "dev/ttyS1" in.
     */
#ifdef _WIN32
    SerialPort serialPort( "\\\\.\\COM1" );

    if ( !serialPort.IsConnected() ) {
        return 0;
    }
#else
    int serialPort = open_port( "/dev/ttyACM0" );

    if ( serialPort == -1 ) {
        return 0;
    }

    set_baud_rate( serialPort , B115200 );
#endif

    float nxyz[sen];
    int ixyz[sen];

    float w = 256; // board size
    bool flip[3] = { false , true , false };

    int player = 0;

    // Setup
    sf::RenderWindow mainWin( sf::VideoMode( 800 , 600 ) , "3D Capacitor Demo" ,
            sf::Style::Titlebar | sf::Style::Close );
    mainWin.setFramerateLimit( 25 );

#if 0
    PFont font;

    size(800, 600, OPENGL);
    frameRate(25);

    font = loadFont("TrebuchetMS-Italic-20.vlw");
    textFont(font);
    textMode(SHAPE);
#endif

    for ( unsigned int i = 0 ; i < sen ; i++ ) {
        cama[i] = new WeightedAverageFilter( 0.01 );

        if ( i == 0 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.10 ); // x (left plate)
        }
        else if ( i == 1 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.10 ); // y (bottom plate)
        }
        else if ( i == 2 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.10 ); // z (right plate)
        }
    }

    resetVariables();

    // Used to store data read from serialPort port
    std::string serialPortData;
    char curChar = '\0';

    sf::Event event;
    while ( mainWin.isOpen() ) {
        while ( mainWin.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                mainWin.close();
            }
            else if ( event.type == sf::Event::KeyPressed ) {
                if ( event.key.code == sf::Keyboard::Tab ) {
                    moves[player * ixyz[0] * ixyz[1]* ixyz[2]] = true;
                    player = player == 0 ? 1 : 0;
                }
            }
            else if ( event.type == sf::Event::MouseButtonPressed ) {
                if ( event.mouseButton.button == sf::Mouse::Right ) {
                    resetVariables();
                }
            }
        }

        // Read line of serialPort data
#ifdef _WIN32
        while ( serialPort.ReadData( &curChar , 1 ) != -1 && curChar != '\n' ) {
            serialPortData += curChar;
        }
#else
        while ( read( serialPort , &curChar , 1 ) != 0 && curChar != '\n' &&
                curChar != '\0' ) {
            serialPortData += curChar;
        }
#endif

        // If curChar == '\n', there is a new line of complete data
        if ( curChar == '\n' ) {
            std::cout << serialPortData << std::endl;

            std::vector<std::string> parts = split( serialPortData , " " );
            if ( parts.size() == sen ) {
                float xyz[3];
                for ( unsigned int i = 0 ; i < sen ; i++ ) {
                    xyz[i] = std::atof( parts[i].c_str() );
                }

                if ( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
                    for( unsigned int i = 0 ; i < sen ; i++ ) {
                        n[i].expandRange( xyz[i] );
                    }
                }

                for ( unsigned int i = 0 ; i < sen ; i++ ) {
                    nxyz[i] = 0.f;
                }

                for ( unsigned int i = 0 ; i < sen ; i++ ) {
                    float raw = n[i].linearize( xyz[i] );

                    nxyz[i] = flip[i] ? 1 - raw : raw;
                    cama[i]->update( nxyz[i] );
                    axyz[i]->update( nxyz[i] );
                    ixyz[i] = getPosition( axyz[i]->getEstimate() );
                }
            }

            // Reset serial data storage in preparation for new line of data
            serialPortData.clear();
            curChar = '\0';
        }

        // Redraw board
        mainWin.clear( sf::Color::White );

#if 0
        float h = w / 2;
        camera(
          h + (cama[0].avg - cama[2].avg) * h,
          h + (cama[1].avg - 1) * height / 2,
          w * 2,
          h, h, h,
          0, 1, 0);

        pushMatrix();

        // Due to a currently unresolved issue with Processing 2.0.3 and OpenGL depth sorting,
        // we can't fill the large box without hiding the rest of the boxes in the scene.
        // We'll use a stroke for this one instead.
        noFill();
        stroke(0, 40);
        translate(w/2, w/2, w/2);
        rotateY(-HALF_PI/2);
        box(w);
        popMatrix();

        float sw = w / subDivs;
        translate(h, sw / 2, 0);
        rotateY(-HALF_PI/2);

        pushMatrix();
        float sd = sw * (subDivs - 1);
        translate(
          axyz[0].getEstimate() * sd,
          axyz[1].getEstimate() * sd,
          axyz[2].getEstimate() * sd);
        fill(255, 160, 0, 200);
        noStroke();
        sphere(18);
        popMatrix();

        for(int z = 0; z < subDivs; z++) {
          for(int y = 0; y < subDivs; y++) {
            for(int x = 0; x < subDivs; x++) {
              pushMatrix();
              translate(x * sw, y * sw, z * sw);

              noStroke();
              if(moves[0][x][y][z])
                fill(255, 0, 0, 200); // transparent red
              else if(moves[1][x][y][z])
                fill(0, 0, 255, 200); // transparent blue
              else if(
              x == ixyz[0] &&
                y == ixyz[1] &&
                z == ixyz[2])
                if(player == 0)
                  fill(255, 0, 0, 200); // transparent red
                else
                  fill(0, 0, 255, 200); // transparent blue
              else
                fill(0, 100); // transparent grey
              box(sw / 3);

              popMatrix();
            }
          }
        }

        stroke(0);
#endif

        mainWin.display();

        if ( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
            msg( "defining boundaries" );
        }
    }

    for ( unsigned int i = 0 ; i < sen ; i++ ) {
        delete cama[i];
        delete axyz[i];
    }

#ifndef _WIN32
    close ( serialPort );
#endif

    return 0;
}

#ifndef _WIN32
int open_port( const char* portLabel ) {
    std::string name( portLabel );

    int fd = open( name.c_str() , O_RDONLY | O_NOCTTY | O_NDELAY );

    if ( fd == -1 ) {
        // Couldn't open the port
        std::string str( __FUNCTION__ );
        str += ": Unable to open " + name;
        perror( str.c_str() );
    }
    else {
        fcntl( fd , F_SETFL , FNDELAY );
    }

    return fd;
}

void set_baud_rate( int fd , speed_t speed ) {
    struct termios options;

    // Get the current options for the port
    tcgetattr( fd , &options );

    // Set the baud rates
    cfsetispeed( &options , speed );
    cfsetospeed( &options , speed );

    // Enable the receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);

    // Set the new options for the port
    tcsetattr( fd , TCSANOW , &options );
}
#endif

void msg( std::string msg ) {
    std::cout << msg << std::endl;
}
