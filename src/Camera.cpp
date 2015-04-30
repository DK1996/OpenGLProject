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

vec3 Camera::PickAgainstPlane(float _x, float _y, vec4 _plane)
{
	float nx_Pos = _x / 1280.0f;
	float ny_Pos = _y / 720.0f;

	float sx_Pos = nx_Pos - 0.5f;
	float sy_Pos = ny_Pos - 0.5f;

	float fx_Pos = sx_Pos * 2;
	float fy_Pos = sy_Pos * -2;

	mat4 inv_View_Proj = inverse(m_projectionView);

	vec4 mouse_Pos(fx_Pos, fy_Pos, 1, 1);
	vec4 world_Pos = inv_View_Proj * mouse_Pos;

	world_Pos /= world_Pos.w;

	vec3 cam_Pos = m_worldTransform[3].xyz();
	vec3 dir = normalize(world_Pos.xyz() - cam_Pos);

	float t = -(dot(cam_Pos, _plane.xyz()) + _plane.w) / (dot(dir, _plane.xyz()));

	vec3 result = cam_Pos + dir * t;

	return result;
}