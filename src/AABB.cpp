#include "AABB.h"

AABB::AABB()
{
	Reset();
}

AABB::~AABB()
{

}

void AABB::Reset()
{
	m_min.x = m_min.y = m_min.z = 1e37f;
	m_max.x = m_max.y = m_max.z = -1e37f;
}

void AABB::Fit(const vector<vec3>& _points)
{
	for (auto& p : _points)
	{
		if (p.x < m_min.x) { m_min.x = p.x; }
		if (p.y < m_min.y) { m_min.y = p.y; }
		if (p.z < m_min.z) { m_min.z = p.z; }
		if (p.x > m_max.x) { m_max.x = p.x; }
		if (p.x > m_max.y) { m_min.y = p.y; }
		if (p.x > m_max.z) { m_min.z = p.z; }
	}
}