#include "ColorFunc.h"


void HsvRgb( float hsv[3], float rgb[3] )
{
	float h, s, v;			// hue, sat, value
	float r, g, b;			// red, green, blue
	float i, f, p, q, t;		// interim values


	// guarantee valid input:

	h = hsv[0] / 60.;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;


	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}


	// get an rgb from the hue itself:
	
	i = floor( h );
	f = h - i;
	p = v * ( 1. - s );
	q = v * ( 1. - s*f );
	t = v * ( 1. - ( s * (1.-f) ) );

	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


void ColorRainbow(const float& min_, const float& max_, const float& value, float rgb[3])
{	
	float hsv[3];
	hsv[0] = 240.0 - 240.0*(value-min_)/(max_-min_);
	hsv[1] = 1.0;
	hsv[2] = 1.0;
	HsvRgb(hsv, rgb);
}

void ColorBlueWhiteRed(const float& min_, const float& max_, const float& value, float rgb[3])
{
	float hsv[3];

	const float& middle = (min_ + max_)/2.0;
	/* linear interpolation from blue to white */
	if(value <= middle)
	{
		hsv[0]=240;
		hsv[1]=(middle-value)/(middle-min_);
	}
	/* linear interpolation from white to red */
	else
	{
		hsv[0]=0;
		hsv[1]=(value-middle)/(max_-middle);
	}
	hsv[2] = 1.0;
	HsvRgb(hsv, rgb);
}

void ColorGrayScale(const float& min_, const float& max_, const float& value, float rgb[3])
{
	const float& ratio = (value-min_)/(max_-min_);
	rgb[0]=rgb[1]=rgb[2] = ratio;
}

void ColorHeated(const float& min_, const float& max_, const float& value, float rgb[3])
{
	const float& first = min_+(max_-min_)*1.0/3.0;
	const float& second = min_+(max_-min_)*2.0/3.0;
	/*From black add red gradually */
	if(value >= min_ && value <= first)
	{
		rgb[0]=(value-min_)/(first-min_);
		rgb[1]=0.0;
		rgb[2]=0.0;
	}
	/*add green gradually */
	else if(value > first && value <= second)
	{
		rgb[0]=1.0;
		rgb[1]=(value-first)/(second-first);
		rgb[2]=0.0;
	}
	/*add blue gradually */
	else
	{
		rgb[0]=rgb[1]=1.0;
		rgb[2]=(value-second)/(max_-second);
	}
/*This is just linearly adding separate components, and you can also achieve it by HSV interpolation from black->red->yellow->white */
}
