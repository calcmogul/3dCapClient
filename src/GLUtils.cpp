//=============================================================================
//File Name: GLUtils.cpp
//Description: Provides convenience functions for drawing OpenGL shapes
//Author: Tyler Veness
//=============================================================================

#include "GLUtils.hpp"
#include <GL/glu.h>
#include <cmath>

void drawBox( float width , GLenum fillType ) {
    float height = width, depth = width;

    glPushMatrix();

    // Add equally to x and z to move up and down in finished view
    glTranslatef( -width / 2 , -height / 2 , depth / 2 );

    if ( fillType == GL_FILL ) {
        glBegin( GL_TRIANGLE_STRIP );
        // Front right vertical line
        glVertex3f( width , 0 , 0 );
        glVertex3f( width , height , 0 );

        // Rear right vertical line
        glColor4ub( 128 , 0 , 0 , 255 ); // RIGHT
        glVertex3f( width , 0 , -depth );
        glVertex3f( width , height , -depth );

        // Rear left vertical line
        glColor4ub( 255 , 0 , 0 , 255 ); // BACK
        glVertex3f( 0 , 0 , -depth );
        glVertex3f( 0 , height , -depth );

        // Front left vertical line
        glColor4ub( 255 , 128 , 0 , 255 ); // LEFT
        glVertex3f( 0 , 0 , 0 );
        glVertex3f( 0 , height , 0 );

        // Front right vertical line
        glColor4ub( 255 , 255 , 0 , 255 ); // FRONT
        glVertex3f( width , 0 , 0 );
        glVertex3f( width , height , 0 );
        glEnd();

#if 0
        glBegin( GL_TRIANGLE_STRIP );
        // Top right horizontal line
        glColor4ub( 0 , 128 , 0 , 255 );
        glVertex3f( width , height , 0 );
        glVertex3f( width , height , -depth );

        // Top left horizontal line
        glColor4ub( 0 , 0 , 128 , 255 );
        glVertex3f( 0 , height , 0 );
        glVertex3f( 0 , height , -depth );
        glEnd();

        glBegin( GL_TRIANGLE_STRIP );
        // Bottom right horizontal line
        glColor4ub( 255 , 0 , 255 , 255 );
        glVertex3f( width , 0 , 0 );
        glVertex3f( width , 0 , -depth );

        // Bottom left horizontal line
        glColor4ub( 128 , 0 , 128 , 255 );
        glVertex3f( 0 , 0 , 0 );
        glVertex3f( 0 , 0 , -depth );
        glEnd();
#endif
    }
    else if ( fillType == GL_LINE ) {
        // Front face
        glBegin( GL_LINE_STRIP );
        glVertex3f( width , 0 , 0 );
        glVertex3f( width , height , 0 );
        glVertex3f( 0 , height , 0 );
        glVertex3f( 0 , 0 , 0 );
        glVertex3f( width , 0 , 0 );
        glEnd();

        // Right face
        glBegin( GL_LINE_STRIP );
        glVertex3f( width , 0 , 0 );
        glVertex3f( width , 0 , -depth );
        glVertex3f( width , height , -depth );
        glVertex3f( width , height , 0 );
        glEnd();

        // Rear face
        glBegin( GL_LINE_STRIP );
        glVertex3f( width , 0 , -depth );
        glVertex3f( 0 , 0 , -depth );
        glVertex3f( 0 , height , -depth );
        glVertex3f( width , height , -depth );
        glEnd();

        // Left face
        glBegin( GL_LINE_STRIP );
        glVertex3f( 0 , 0 , -depth );
        glVertex3f( 0 , 0 , 0 );
        glVertex3f( 0 , height , 0 );
        glVertex3f( 0 , height , -depth );
        glEnd();
    }

    glPopMatrix();
}
