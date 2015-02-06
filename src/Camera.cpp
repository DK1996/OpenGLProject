#include "Camera.h"

void Camera::setPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_projectionTransform = perspective(fieldOfView, aspectRatio, near, far);
}

void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	m_viewTransform = lookAt(from, to, up);
	m_worldTransform = inverse(m_viewTransform);
}

void Camera::setPosition(vec3 position)
{
	UpdateProjectionView();
}

void Camera::UpdateProjectionView()
{
	m_viewTransform = inverse(m_worldTransform);
	m_projectionView = m_viewTransform * m_projectionTransform;
}