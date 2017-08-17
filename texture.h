#pragma once

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <SOIL/SOIL.h>
using namespace std;

class Texture
{
public:
    static GLuint num_texture;

     Texture (void);
    /** Destructor.
     */
    GLuint LoadBitmap(const char *filename);
    GLuint png_texture_load(const char * file_name);
    GLuint jpg_texture_load(const char * file_name);

private:
    typedef struct                       /**** BMP file info structure ****/
    {
        unsigned int   biSize;           /* Size of info header */
        int            biWidth;          /* Width of image */
        int            biHeight;         /* Height of image */
        unsigned short biPlanes;         /* Number of color planes */
        unsigned short biBitCount;       /* Number of bits per pixel */
        unsigned int   biCompression;    /* Type of compression to use */
        unsigned int   biSizeImage;      /* Size of image data */
        int            biXPelsPerMeter;  /* X pixels per meter */
        int            biYPelsPerMeter;  /* Y pixels per meter */
        unsigned int   biClrUsed;        /* Number of colors used */
        unsigned int   biClrImportant;   /* Number of important colors */
        char *data;
    } BITMAPINFOHEADER;

};
