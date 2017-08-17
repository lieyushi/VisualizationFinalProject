#include "Data.h"

Data::Data()
{
}
	
Data::~Data()
{
}
	
Data::Data(const string& line)
{
	stringstream ss(line);
	string temp;

	ss >> temp;
	position.x = atof(temp.c_str());

	ss >> temp;
	position.y = atof(temp.c_str());

	ss >> temp;
	position.z = atof(temp.c_str());

	ss >> temp;
	velocity.x = atof(temp.c_str());

	ss >> temp;
	velocity.y = atof(temp.c_str());

	ss >> temp;
	velocity.z = atof(temp.c_str());

	ss >> temp;
	density = atof(temp.c_str());

	ss >> temp;
	neighbor = atoi(temp.c_str());

	ss >> temp;
	v_norm_gradient.x = atof(temp.c_str());

	ss >> temp;
	v_norm_gradient.y = atof(temp.c_str());

	ss >> temp;
	v_norm_gradient.z = atof(temp.c_str());

	ss >> temp;
	density_gradient.x = atof(temp.c_str());

	ss >> temp;
	density_gradient.y = atof(temp.c_str());

	ss >> temp;
	density_gradient.z = atof(temp.c_str());

	ss >> temp;
	ftle = atof(temp.c_str());

	ss >> temp;
	rotation = atof(temp.c_str());

	ss.clear();

	velo_norm = getNorm(velocity);
	dv_norm = getNorm(v_norm_gradient);
	den_norm = getNorm(density_gradient);
}

const float getNorm(const vec3& velo)
{
	return sqrt(velo.x*velo.x+velo.y*velo.y+velo.z*velo.z);
}