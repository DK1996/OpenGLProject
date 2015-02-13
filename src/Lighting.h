#ifndef LIGHTING_H
#define LIGHTING_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"

using namespace std;

class Lighting : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void CreateOpenGLBuffers(vector<tinyobj::shape_t> &shapes);
	void CleanUpOpenGLBuffers();
	
	void ReloadShader();

	Camera* m_camera;

	unsigned int program_ID;

	float dt;

	vector<OpenGLData> m_glData;

	vec3 light_Dir;
	vec3 light_Color;
	vec3 ambient_Light;
	vec3 material_Color;

	float specular_Power;
};

#endif