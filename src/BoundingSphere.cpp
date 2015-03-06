#include "BoundingSphere.h"

BoundingSphere::BoundingSphere() : m_centre(0), m_radius(0)
{

}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Fit(const vector<vec3>& _points)
{
	vec3 m_min(1e37f);
	vec3 m_max(-1e37f);

	for (auto& p : _points)
	{
		if (p.x < m_min.x) { m_min.x = p.x; }
		if (p.y < m_min.y) { m_min.y = p.y; }
		if (p.z < m_min.z) { m_min.z = p.z; }
		if (p.x > m_max.x) { m_max.x = p.x; }
		if (p.x > m_max.y) { m_max.y = p.y; }
		if (p.x > m_max.z) { m_max.z = p.z; }
	}

	m_centre = (m_min + m_max) * 0.5f;
	m_radius = glm::distance(m_min, m_centre);

}
