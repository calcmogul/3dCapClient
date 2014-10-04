//=============================================================================
//File Name: Main.cpp
//Description: Implements mouse input driver using 3D capacitor
//Author: Tyler Veness
//=============================================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

#include "SerialPort.hpp"

#include "Normalize.hpp"
#include "WeightedAverageFilter.hpp"
#include "KalmanFilter.hpp"
#include "Util.hpp"
#include "GLUtils.hpp"

#include <SFML/Window/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <GL/gl.h>
#include <GL/glu.h>

void msg( std::string msg );

const unsigned int sen = 3; // sensors
const unsigned int subDivs = 3; // board sub-subDivsisions

Normalize n[sen];
WeightedAverageFilter* cama[sen];
KalmanFilter* axyz[sen];

void resetVariables() {
    for( unsigned int i = 0 ; i < sen ; i++ ) {
        n[i].reset();
        cama[i]->reset();
        axyz[i]->reset();
    }
}

#if 0
float cutoff = 0.2;
int getPosition( float x ) {
    if ( subDivs == 3 ) {
        if ( x < cutoff ) { // 0 < x < 0.2
            return 0;
        }
        else if ( x < 1 - cutoff ) { // 0.2 < x < 0.8
            return 1;
        }
        else { // 0.8 < x < 1.2
            return 2;
        }
    }
    else {
        return x == 1 ? subDivs - 1 : (int) x * subDivs;
    }
}
#endif

int main() {
    /* The argument to open_port should be the serial port of your Arduino.
     *
     * i.e. in Windows, if you have 3 ports: COM1, COM2, COM3 and your Arduino
     * is on COM2, pass "COM2" in.
     *
     * i.e. in Linux, if you have 3 ports: /dev/ttyS0, /dev/ttyS1, /dev/tty2
     * and your Arduino is on ttyS1, pass "dev/ttyS1" in.
     */
    // FIXME: test that there is at least one serial port in the list
    SerialPort serialPort( SerialPort::getSerialPorts()[0].c_str() );

    float nxyz[sen];
    unsigned int ixyz[sen];

    float w = 256; // board size
    bool flip[3] = { false , true , false };

    // Setup
    sf::Window mainWin( sf::VideoMode( 800 , 600 ) , "3D Capacitor Demo" ,
            sf::Style::Titlebar | sf::Style::Close );
    mainWin.setFramerateLimit( 25 );

    /* ===== Initialize OpenGL ===== */
    glClearColor( 1.f , 1.f , 1.f , 1.f );
    glClearDepth( 0.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDepthFunc( GL_LESS );
    glDepthMask( GL_TRUE );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glEnable( GL_BLEND );
    glEnable( GL_ALPHA_TEST );
    glDisable( GL_TEXTURE_2D );
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    glShadeModel( GL_FLAT );

    // Set up screen
    glViewport( 0 , 0 , 800 , 600 );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -400 , 400 , -300 , 300 , -1000.f , 1000.f );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    /* ============================= */

    for ( unsigned int i = 0 ; i < sen ; i++ ) {
        cama[i] = new WeightedAverageFilter( 0.04 );

        if ( i == 0 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.0001 ); // x (left plate)
        }
        else if ( i == 1 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.0001 ); // y (bottom plate)
        }
        else if ( i == 2 ) {
            axyz[i] = new KalmanFilter( 0.00006 , 0.0001 ); // z (right plate)
        }
    }

    resetVariables();

    // Used to store data read from serialPort port
    std::string serialPortData;
    char curChar = '\0';
    char numRead = 0;

    sf::Event event;
    while ( mainWin.isOpen() ) {
        while ( mainWin.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                mainWin.close();
            }
            else if ( event.type == sf::Event::MouseButtonPressed ) {
                if ( event.mouseButton.button == sf::Mouse::Right ) {
                    resetVariables();
                }
            }
        }

        // Read line of serialPort data
        if ( serialPort.isConnected() ) {
            while ( (numRead = serialPort.read( &curChar , 1 )) > 0 && curChar != '\n' &&
                    curChar != '\0' ) {
                serialPortData += curChar;
            }

            if ( numRead == 0 || (numRead == -1 && errno != EAGAIN) ) {
                // EOF has been reached (socket disconnected)
                serialPort.disconnect();
            }
            // If curChar == '\n', there is a new line of complete data
            else if ( curChar == '\n' && serialPortData.length() != 0 ) {
                std::cout << "\"" << serialPortData << "\"\n";

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

                        /* Converts normalized position estimate [0..1] to
                         * position in array [0..2]
                         */
                        ixyz[i] = std::lround( axyz[i]->getEstimate() * 2.0 );
                        //ixyz[i] = getPosition( axyz[i]->getEstimate() );
                    }
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
                curChar = '\0';
                numRead = 0;
            }
        }
        else {
            serialPort.connect( SerialPort::getSerialPorts()[0].c_str() );
        }

        // Clear the buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glPushMatrix();

        /* The sensor's coordinate axes are oriented differently from OpenGL's
         * axes, so rotate the view until they match.
         */
        glRotatef( 180.f , 1.f , 0.f , 0.f );

        gluLookAt(
          w / 2 + (cama[0]->getEstimate() - cama[2]->getEstimate()) * w / 2,
          w / 2 + (cama[1]->getEstimate() - 1) * 600 / 2,
          w * 2,
          w / 2, w / 2, w / 2,
          0, 1, 0);

        glPushMatrix();

        // Draw outer boundary box
        glColor4ub( 0 , 0 , 0 , 40 );
        glTranslatef( w / 2 , w / 2 , w / 2 );
        glRotatef( -45.f , 0.f , 1.f , 0.f );
        drawBox( w , GL_LINE );

        glPopMatrix();

        float sw = w / subDivs;

        glTranslatef( w / 2 , sw / 2 , 0 );
        glRotatef( -45.f , 0.f , 1.f , 0.f );

        glPushMatrix();
        float sd = sw * (subDivs - 1);

        glTranslatef(
          axyz[0]->getEstimate() * sd,
          axyz[1]->getEstimate() * sd,
          axyz[2]->getEstimate() * sd);

        // Draw sphere for current position of hand
        glColor4ub( 255 , 160 , 0 , 200 );
        GLUquadricObj* sphere = gluNewQuadric();
        if ( sphere != NULL ) {
            gluQuadricNormals( sphere , GLU_SMOOTH );
            gluSphere( sphere , 18 , 32 , 32 );
            gluDeleteQuadric( sphere );
        }

        glPopMatrix();

        for ( unsigned int z = 0 ; z < subDivs ; z++ ) {
            for ( unsigned int y = 0 ; y < subDivs ; y++ ) {
                for ( unsigned int x = 0 ; x < subDivs ; x++ ) {
                    glPushMatrix();

                    glTranslatef( x * sw , y * sw , z * sw );

                    if (    x == ixyz[0] &&
                            y == ixyz[1] &&
                            z == ixyz[2]) {
                        glColor4ub( 255 , 0 , 0 , 200 ); // transparent red
                    }
                    else {
                        glColor4ub( 100 , 100 , 100 , 100 ); // transparent gray
                    }
                    drawBox( sw / 3 , GL_FILL );

                    glPopMatrix();
                }
            }
        }

        glPopMatrix();

        mainWin.display();

        if ( sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
            //msg( "defining boundaries" );
        }
    }

    for ( unsigned int i = 0 ; i < sen ; i++ ) {
        delete cama[i];
        delete axyz[i];
    }

    return 0;
}

void msg( std::string msg ) {
    std::cout << msg << '\n';
}
