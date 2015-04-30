#ifndef CAMERA_H
#define CAMERA_H

#include "glm_header.h"

class Camera
{
public:
	Camera() {}
	Camera(float aspect);
	~Camera() {}

	virtual void Update(float deltaTime) = 0;
	void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void setLookAt(vec3 from, vec3 to, vec3 up);
	void setPosition(vec3 position);
	
	void UpdateProjectionView();

	vec3 PickAgainstPlane(float _x, float _y, vec4 _plane);

	mat4 m_worldTransform;
	mat4 m_viewTransform;
	mat4 m_projectionTransform;
	mat4 m_projectionView;

private:
	
};

#endif
