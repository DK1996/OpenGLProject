#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H

#include "glm_header.h"
#include <vector>

using namespace std;

class BoundingSphere
{
public:

	BoundingSphere();
	~BoundingSphere();

	void Fit(const vector<vec3>& _points);

	vec3 m_centre;
	float m_radius;
};

#endif // !BOUNDING_SPHERE_H
