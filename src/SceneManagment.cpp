#include "SceneManagment.h"
#include "FlyCamera.h"
#include "BoundingSphere.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>

bool SceneManagment::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void SceneManagment::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool SceneManagment::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);


	m_camera->Update(dt);

	vec4 plane(0, 1, 0, -1);
	
	float d = glm::dot(vec3(plane), vec3(4, 2, 0)) + plane.w;
	
	if (d < 0) { printf("Plane back\n"); }
	else if (d > 0){ printf("Plane front\n"); }
	else { printf("Plane on the plane\n"); }
	
	m_sphere.m_centre = vec3(0, cosf((float)glfwGetTime()) + 1, 0);
	m_sphere.m_radius = 0.5f;
	
	float e = dot(vec3(plane), m_sphere.m_centre) + plane.w;
	
	if (e > m_sphere.m_radius) { printf("Sphere front\n"); }
	else if (e < -m_sphere.m_radius) { printf("Sphere back\n"); }
	else { printf("Sphere on the plane\n"); }
	
	Gizmos::addSphere(m_sphere.m_centre, m_sphere.m_radius, 8, 8, vec4(1, 0, 1, 1));
	
	vec4 plane_Color(1, 1, 0, 1);
	
	if (e > m_sphere.m_radius)
	{
		plane_Color = vec4(0, 1, 0, 1);
	}
	else if (e < -m_sphere.m_radius)
	{
		plane_Color = vec4(1, 0, 0, 0);
	}
	
	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), plane_Color);
	Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), plane_Color);
	
	vec4 planes[6];
	GetFrustumPlanes(m_camera->m_projectionView, planes);
	
	for (int i = 0; i < 6; i++)
	{
		float d = dot(vec3(planes[i]), m_sphere.m_centre) + planes[i].w;
	
		if (d < -m_sphere.m_radius)
		{
			printf("Behind, don't render it!\n");
			break;
		}
		else if (d < m_sphere.m_radius)
		{
			printf("Touching, we still need to render it!\n");
		}
		else
		{
			printf("Front, fully visible so render it!\n");
		}
	}

	return true;
}

void SceneManagment::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();

	Application::Draw();
}

void SceneManagment::GetFrustumPlanes(const mat4& _transform, vec4* _planes)
{
	// Right Side.
	_planes[0] = vec4( _transform[0][3] - _transform[1][0],
					   _transform[1][3] - _transform[1][0],
					   _transform[2][3] - _transform[2][0],
					   _transform[3][3] - _transform[3][0] );

	// Left Side.
	_planes[1] = vec4( _transform[0][3] + _transform[0][0],
					   _transform[1][3] + _transform[1][0],
					   _transform[2][3] + _transform[2][0],
					   _transform[3][3] + _transform[3][0] );

	// Top.
	_planes[0] = vec4( _transform[0][3] - _transform[0][1],
					   _transform[1][3] - _transform[1][1],
					   _transform[2][3] - _transform[2][1],
					   _transform[3][3] - _transform[3][1] );

	// Bottom.
	_planes[0] = vec4( _transform[0][3] + _transform[0][1],
					   _transform[1][3] + _transform[1][1],
					   _transform[2][3] + _transform[2][1],
					   _transform[3][3] + _transform[3][1] );

	// Far.
	_planes[0] = vec4( _transform[0][3] - _transform[0][2],
					   _transform[1][3] - _transform[1][2],
					   _transform[2][3] - _transform[2][2],
					   _transform[3][3] - _transform[3][2] );

	// Near.
	_planes[0] = vec4( _transform[0][3] + _transform[0][2],
					   _transform[1][3] + _transform[1][2],
					   _transform[2][3] + _transform[2][2],
					   _transform[3][3] + _transform[3][2] );

	for (int i = 0; i < 6; i++)
	{
		_planes[i] = normalize(_planes[i]);
	}
}