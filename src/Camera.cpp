#include "Camera.h"

Camera::Camera(float aspect)
{
	m_viewTransform = lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_worldTransform = inverse(m_projectionView);
	UpdateProjectionView();
}

void Camera::setPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_projectionTransform = perspective(fieldOfView, aspectRatio, near, far);
}

void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	m_viewTransform = lookAt(from, to, up);
	m_worldTransform = inverse(m_viewTransform);
	UpdateProjectionView();
}

void Camera::setPosition(vec3 position)
{
	UpdateProjectionView();
}

void Camera::UpdateProjectionView()
{
	m_viewTransform = inverse(m_worldTransform);
	m_projectionView = m_projectionTransform * m_viewTransform;
}