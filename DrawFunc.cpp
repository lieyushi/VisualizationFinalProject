#include "DrawFunc.h"

void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}


void Axes( float length )
{
    int i, j;           // counters
    float fact;         // character scale factor
    float base;         // character start location


    glBegin( GL_LINE_STRIP );
        glVertex3f( length, 0., 0. );
        glVertex3f( 0., 0., 0. );
        glVertex3f( 0., length, 0. );
    glEnd();
    glBegin( GL_LINE_STRIP );
        glVertex3f( 0., 0., 0. );
        glVertex3f( 0., 0., length );
    glEnd();

    fact = LENFRAC * length;
    base = BASEFRAC * length;

    glBegin( GL_LINE_STRIP );
        for( i = 0; i < 4; i++ )
        {
            j = xorder[i];
            if( j < 0 )
            {
                
                glEnd();
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
        }
    glEnd();

    glBegin( GL_LINE_STRIP );
        for( i = 0; i < 5; i++ )
        {
            j = yorder[i];
            if( j < 0 )
            {
                
                glEnd();
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
        }
    glEnd();

    glBegin( GL_LINE_STRIP );
        for( i = 0; i < 6; i++ )
        {
            j = zorder[i];
            if( j < 0 )
            {
                
                glEnd();
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
        }
    glEnd();

}



void InitLists( GLuint  &axesList, GLuint &boxList )
{
    float dx = BOXSIZE / 2.;
    float dy = BOXSIZE / 2.;
    float dz = BOXSIZE / 2.;

    // create the object:

    boxList = glGenLists( 1 );
    glNewList( boxList, GL_COMPILE );

        glBegin( GL_QUADS );

            glColor3f( 0., 0., 1. );
            glNormal3f( 0., 0.,  1. );
                glVertex3f( -dx, -dy,  dz );
                glVertex3f(  dx, -dy,  dz );
                glVertex3f(  dx,  dy,  dz );
                glVertex3f( -dx,  dy,  dz );

            glNormal3f( 0., 0., -1. );
                glTexCoord2f( 0., 0. );
                glVertex3f( -dx, -dy, -dz );
                glTexCoord2f( 0., 1. );
                glVertex3f( -dx,  dy, -dz );
                glTexCoord2f( 1., 1. );
                glVertex3f(  dx,  dy, -dz );
                glTexCoord2f( 1., 0. );
                glVertex3f(  dx, -dy, -dz );

            glColor3f( 1., 0., 0. );
            glNormal3f(  1., 0., 0. );
                glVertex3f(  dx, -dy,  dz );
                glVertex3f(  dx, -dy, -dz );
                glVertex3f(  dx,  dy, -dz );
                glVertex3f(  dx,  dy,  dz );

            glNormal3f( -1., 0., 0. );
                glVertex3f( -dx, -dy,  dz );
                glVertex3f( -dx,  dy,  dz );
                glVertex3f( -dx,  dy, -dz );
                glVertex3f( -dx, -dy, -dz );

            glColor3f( 0., 1., 0. );
            glNormal3f( 0.,  1., 0. );
                glVertex3f( -dx,  dy,  dz );
                glVertex3f(  dx,  dy,  dz );
                glVertex3f(  dx,  dy, -dz );
                glVertex3f( -dx,  dy, -dz );

            glNormal3f( 0., -1., 0. );
                glVertex3f( -dx, -dy,  dz );
                glVertex3f( -dx, -dy, -dz );
                glVertex3f(  dx, -dy, -dz );
                glVertex3f(  dx, -dy,  dz );

        glEnd();

    glEndList();


    // create the axes:

    axesList = glGenLists( 1 );
    glNewList( axesList, GL_COMPILE );
        glLineWidth( AXES_WIDTH );
            Axes( 1.5 );
        glLineWidth( 1. );
    glEndList();
}