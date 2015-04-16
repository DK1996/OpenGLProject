#ifndef _FLY_CAMERA_H_
#define _FLY_CAMERA_H_

#include "Camera.h"
#include <GLFW/glfw3.h>

class FlyCamera : public Camera
{
public:
	void Update(float _deltaTime)
	{
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
		{
			m_worldTransform[3] += m_worldTransform[2] * m_speed * _deltaTime;
		}
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
		{
			m_worldTransform[3] -= m_worldTransform[2] * m_speed * _deltaTime;
		}
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
		{
			m_worldTransform[3] -= m_worldTransform[1] * m_speed * _deltaTime;
		}
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
		{
			m_worldTransform[3] += m_worldTransform[1] * m_speed * _deltaTime;
		}
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
		{
			m_worldTransform[3] -= m_worldTransform[0] * m_speed * _deltaTime;
		}
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
		{
			m_worldTransform[3] += m_worldTransform[0] * m_speed * _deltaTime;
		}

		double x_delta, y_delta;

		glfwGetCursorPos(glfwGetCurrentContext(), &x_delta, &y_delta);
		

		x_delta -= (1280.0f / 2.0f);
		y_delta -= (720.0f / 2.0f);

		x_delta /= (1280.0f / 2.0f);
		y_delta /= (720.0f / 2.0f);

		x_delta *= m_sensitivity;
		y_delta *= m_sensitivity;

		if (glfwGetMouseButton(glfwGetCurrentContext(), 1))
		{
			glfwSetCursorPos(glfwGetCurrentContext(), 1280.0f / 2.0f, 720.0f / 2.0f);
			vec3 camera_right = (vec3)m_worldTransform[0];

			mat4 yaw = rotate((float)x_delta, vec3(0, 1, 0));
			mat4 pitch = rotate((float)y_delta, camera_right);
			mat4 rot = yaw * pitch;

			m_worldTransform[0] = rot * m_worldTransform[0];
			m_worldTransform[1] = rot * m_worldTransform[1];
			m_worldTransform[2] = rot * m_worldTransform[2];
		}

		m_worldTransform[3][3] = 1;

		m_viewTransform = inverse(m_worldTransform);
		this->UpdateProjectionView();
	}

	void setSpeed(float _speed) { m_speed = _speed; }
	
	float m_speed = 10.0f;
	float m_sensitivity = -2.5f;

	vec3 m_up;

};

#endif