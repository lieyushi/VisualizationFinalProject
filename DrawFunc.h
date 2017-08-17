#ifndef _DRAWFUNC_H
#define _DRAWFUNC_H

#include <GL/glut.h>

const float LENFRAC = 0.10f;
const float BASEFRAC = 1.10f;
static int xorder[] = 
	{
		1, 2, -3, 4
	};

static int yorder[] =
	{
		1, 2, 3, -2, 4
	};

static int zorder[] = 
	{
		1, 2, 3, 4, -5, 6
	};

static float xx[] = 
	{
		0.f, 1.f, 0.f, 1.f
	};

static float xy[] = {
		-.5f, .5f, .5f, -.5f
	      };

static float yx[] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[] = {
		0.f, .6f, 1.f, 1.f
	      };

static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

const float BOXSIZE = { 40.f };
const GLfloat AXES_WIDTH   = { 10. };

void drawString(const char *str, int x, int y, float color[4], void *font);
void Axes( float length );
void InitLists( GLuint	&axesList, GLuint &boxList);

#endif