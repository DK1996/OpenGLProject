#ifndef AABB_H
#define	AABB_H

#include <vector>

using namespace std;

class AABB
{
public:

	AABB();
	~AABB();

	void Reset();
	void Fit(const vector<vec3>& _points);
	
	vec3 m_min, m_max;

};

#endif // !SCENE_MANAGMENT_H
