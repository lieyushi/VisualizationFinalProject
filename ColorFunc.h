#ifndef _COLORFUNC_H
#define _COLORFUNC_H
#include <math.h> 

void ColorRainbow(const float& min_, const float& max_, const float& value, float rgb[3]);
void ColorBlueWhiteRed(const float& min_, const float& max_, const float& value, float rgb[3]);
void ColorGrayScale(const float& min_, const float& max_, const float& value, float rgb[3]);
void ColorHeated(const float& min_, const float& max_, const float& value, float rgb[3]);
void HsvRgb( float hsv[3], float rgb[3] );

#endif