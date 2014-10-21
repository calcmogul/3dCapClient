//=============================================================================
//File Name: Main.cpp
//Description: Implements mouse input driver using 3D capacitor
//Author: Tyler Veness
//=============================================================================

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

//#define USE_RAW_INPUT

int main() {
    const unsigned int sen = 3; // sensors
    const unsigned int subDivs = 3; // board sub-divisions

    // x (left plate), y (bottom plate), z (right plate)
    std::vector<Normalize> n( sen );
    std::vector<WeightedAverageFilter> cama( sen , WeightedAverageFilter(0.04) );
    std::vector<KalmanFilter> axyz( sen , KalmanFilter(0.00006 , 0.0001) );

    bool haveValidData = false;
    bool calibrate = false;

    SerialPort serialPort;

    unsigned int ixyz[sen];

    float w = 256; // board size
    bool flip[sen] = { false , true , false };

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
    sf::Window mainWin( sf::VideoMode( 800 , 600 ) , "3D Capacitor Demo" ,
            sf::Style::Titlebar | sf::Style::Close , settings );
    mainWin.setFramerateLimit( 25 );

    sf::Vector2i mouseDragStart = sf::Mouse::getPosition( mainWin );
    Matrix<GLfloat> rotationMat( 4 , 4 ); // column x row

    // Set buffer clear values
    glClearColor( 1.f , 1.f , 1.f , 1.f );
    glClearDepth( 1.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Enable depth buffering
    glDepthFunc( GL_LESS );
    glDepthMask( GL_TRUE );
    glEnable( GL_DEPTH_TEST );

    // Enable alpha blending
    glEnable( GL_BLEND );
    glEnable( GL_ALPHA_TEST );
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // Disable textures since they are unneeded
    glDisable( GL_TEXTURE_2D );

    // Declare lighting parameters
    GLfloat mat_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0 , 1.0 , 1.0 , 0.0 };

    // Set lighting parameters
    glShadeModel( GL_SMOOTH );
    glMaterialfv( GL_FRONT , GL_SPECULAR , mat_specular );
    glMaterialfv( GL_FRONT , GL_SHININESS , mat_shininess );
    glLightfv( GL_LIGHT0 , GL_POSITION , light_position );

    // Enable lighting
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT , GL_AMBIENT_AND_DIFFUSE );

    // Set up screen
    glViewport( 0 , 0 , mainWin.getSize().x , mainWin.getSize().y );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.f ,
            static_cast<float>(mainWin.getSize().x) / mainWin.getSize().y ,
            250.f ,
            900.f );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

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
            else if ( event.type == sf::Event::KeyPressed ) {
                if ( event.key.code == sf::Keyboard::Space ) {
                    calibrate = true;
                }
            }
            else if ( event.type == sf::Event::KeyReleased ) {
                if ( event.key.code == sf::Keyboard::Space ) {
                    calibrate = false;
                }
            }
            else if ( event.type == sf::Event::MouseButtonPressed ) {
                if ( event.mouseButton.button == sf::Mouse::Right ) {
                    // Reset filters
                    for( unsigned int i = 0 ; i < sen ; i++ ) {
                        n[i].reset();
                        cama[i].reset();
                        axyz[i].reset();
                    }
                }
            }
            else if ( event.type == sf::Event::MouseMoved ) {
                Matrix<GLfloat> tempMat( 4 , 4 );

                float mag = std::hypot( event.mouseMove.x , event.mouseMove.y );
                float x = event.mouseMove.x;
                float y = event.mouseMove.y;
                float angle = mag;
                float c = std::cos( angle );
                float s = std::sin( angle );
                x /= mag;
                y /= mag;

                /*tempMat[0][0] = x * x * ( 1 - c ) + c;
                tempMat[0][1] = y * x * ( 1 - c ) + z * s;
                tempMat[0][2] = x * z * ( 1 - c ) - y * s;
                tempMat[0][3] = 0;
                tempMat[1][0] = x * y * ( 1 - c ) - z * s;
                tempMat[1][1] = y * y * ( 1 - c ) + c;
                tempMat[1][2] = y * z * ( 1 - c ) + x * s;
                tempMat[1][3] = 0;
                tempMat[2][0] = x * z * ( 1 - c ) + y * s;
                tempMat[2][1] = y * z * ( 1 - c ) - x * s;
                tempMat[2][2] = z * z * ( 1 - c ) + c;
                tempMat[2][3] = 0;
                tempMat[3][0] = 0;
                tempMat[3][1] = 0;
                tempMat[3][2] = 0;
                tempMat[3][3] = 1;*/
                tempMat[0][0] = x * x * ( 1 - c ) + c;
                tempMat[0][1] = y * x * ( 1 - c );
                tempMat[0][2] = -y * s;
                tempMat[0][3] = 0;
                tempMat[1][0] = x * y * ( 1 - c );
                tempMat[1][1] = y * y * ( 1 - c ) + c;
                tempMat[1][2] = x * s;
                tempMat[1][3] = 0;
                tempMat[2][0] = y * s;
                tempMat[2][1] = -x * s;
                tempMat[2][2] = c;
                tempMat[2][3] = 0;
                tempMat[3][0] = 0;
                tempMat[3][1] = 0;
                tempMat[3][2] = 0;
                tempMat[3][3] = 1;


            }
        }

        // Attempt a connection
        if ( !serialPort.isConnected() ) {
            std::vector<std::string>&& ports = SerialPort::getSerialPorts();
            if ( ports.size() > 0 ) {
                serialPort.connect( ports[0] );
            }
        }

        // Read line of serialPort data
        if ( serialPort.isConnected() ) {
            while ( (numRead = serialPort.read( &curChar , 1 )) > 0 && curChar != '\n' &&
                    curChar != '\0' ) {
                serialPortData += curChar;
            }

            if ( numRead == -1 ) {
                // EOF has been reached (socket disconnected)
                serialPort.disconnect();
            }
            // If curChar == '\n', there is a new line of complete data
            else if ( curChar == '\n' && serialPortData.length() != 0 ) {
                std::cout << "\"" << serialPortData << "\"\n";

                std::vector<std::string> parts = split( serialPortData , " " );

                if ( parts.size() == sen ) {
#ifndef USE_RAW_INPUT
                    float xyz[sen];
#endif
                    float raw;

                    haveValidData = true;

                    for ( unsigned int i = 0 ; i < sen ; i++ ) {
                        xyz[i] = std::atof( parts[i].c_str() );

                        if ( calibrate ) {
                            n[i].expandRange( xyz[i] );
                            std::cout << "defining boundaries\n";
                        }

                        raw = n[i].linearize( xyz[i] );

                        // Update camera and position filters
                        if ( flip[i] ) {
                            cama[i].update( 1 - raw );
                            axyz[i].update( 1 - raw );
                        }
                        else {
                            cama[i].update( raw );
                            axyz[i].update( raw );
                        }

                        /* Converts normalized position estimate [0..1] to
                         * position in array [0..subDivs-1]
                         */
                        ixyz[i] = std::lround( axyz[i].getEstimate() * (subDivs - 1) );
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

        // Clear the buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glPushMatrix();

        // Draw connection indicator
        glPushMatrix();

        gluLookAt(
          0.f , 0.f , w * 2 ,
          0.f , 0.f , w / 2 ,
          0 , 1 , 0 );

        glTranslatef(
                36.f - static_cast<int>(mainWin.getSize().x) / 2.f ,
                -36.f + static_cast<int>(mainWin.getSize().y) / 2.f ,
                0.f );

        if ( serialPort.isConnected() ) {
            if ( haveValidData ) {
                // Connected and valid data
                glColor3ub( 0 , 200 , 0 );
            }
            else {
                // Connected but no valid data
                glColor3ub( 255 , 220 , 0 );
            }
        }
        else {
            // Disconnected
            glColor3ub( 200 , 0 , 0 );
        }

        //glShadeModel( GL_FLAT );
        glDisable( GL_LIGHTING );
        drawCircle( 18.f , 32.f );
        glEnable( GL_LIGHTING );
        //glShadeModel( GL_SMOOTH );

        glPopMatrix();

        gluLookAt(
          w / 2 + (cama[0].getEstimate() - cama[2].getEstimate()) * w / 2 ,
          (w * 3 + (cama[1].getEstimate() - 1) * mainWin.getSize().y / 2) ,
          w * 2 ,
          w / 2 , w / 2 , w / 2 ,
          0 , 1 , 0 );

        /* The sensor's coordinate axes are oriented differently from OpenGL's
         * axes, so rotate the view until they match. glTranslatef() is used to
         * make rotation occur around center of cube. This translation is
         * undone manually since pushing and popping a matrix would undo the
         * rotation as well.
         */
        glTranslatef( w / 2 , w / 2 , w / 2 );
        glRotatef( 180.f , 1.f , 0.f , 0.f );
        float* data = rotationMat.data();
        glMultMatrixf( data ); // Rotate view with mouse
        delete data;
        glTranslatef( -w / 2 , -w / 2 , -w / 2 );

        glPushMatrix();

        // Draw outer boundary box
        glColor4ub( 0 , 0 , 0 , 40 );
        glTranslatef( w / 2 , w / 2 , w / 2 );
        glRotatef( -45.f , 0.f , 1.f , 0.f );
        drawBox( w , GL_LINE );

        glPopMatrix();

        float subDivWidth = w / subDivs;

        glTranslatef( w / 2 , subDivWidth / 2 , 0 );
        glRotatef( -45.f , 0.f , 1.f , 0.f );

        glPushMatrix();

        /* Converts normalized average to position within cube
         *   axyz * (subDivs - 1) * (w / subDivs)
         * = axyz * (w - w / subDivs)
         * = axyz * ( w - subDivWidth)
         */
        float posModifier = w - subDivWidth;

#ifdef USE_RAW_INPUT
        float raw[sen];
        for ( unsigned int i = 0 ; i < sen ; i++ ) {
            raw[i] = n[i].linearize( xyz[i] );

            if ( flip[i] ) {
                raw[i] = 1 - raw[i];
            }
        }

        glTranslatef(
                  raw[0] * posModifier ,
                  raw[1] * posModifier ,
                  raw[2] * posModifier );
#else
        glTranslatef(
          axyz[0].getEstimate() * posModifier ,
          axyz[1].getEstimate() * posModifier ,
          axyz[2].getEstimate() * posModifier );
#endif

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

                    glTranslatef( x * subDivWidth , y * subDivWidth , z * subDivWidth );

                    if (    x == ixyz[0] &&
                            y == ixyz[1] &&
                            z == ixyz[2]) {
                        // transparent red
                        glColor4ub( 255 , 0 , 0 , 200 );
                    }
                    else {
                        // transparent gray
                        glColor4ub( 100 , 100 , 100 , 100 );
                    }
                    drawBox( subDivWidth / 3 , GL_FILL );

                    glPopMatrix();
                }
            }
        }

        glPopMatrix();

        mainWin.display();
    }

    return 0;
}
