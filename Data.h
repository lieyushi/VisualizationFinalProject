#ifndef _DATA_H
#define _DATA_H

#include <stdlib.h> 
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace glm;
using namespace std;


class Data
{
public:
	vec3 position;
	vec3 velocity;
	float velo_norm;

	float density;
	int neighbor;

	vec3 v_norm_gradient;
	float dv_norm;

	vec3 density_gradient;
	float den_norm;
	
	float ftle;
	float rotation;

	Data();
	~Data();
	Data(const string& line);
	
};

const float getNorm(const vec3& velo);

#endif