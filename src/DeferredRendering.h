#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "AntTweakBar.h"

class DeferredRendering : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void BuildMesh();

	void BuildGBuffer();
	void BuildLightBuffer();

	void BuildQuad();
	void BuildCube();

	void RenderDirectionalLight(vec3 _light_Dir, vec3 _light_Color);
	void RenderPointLight(vec3 _position, float _radius, vec3 _diffuse);

	void ReloadShader();

	// Load mesh.
	OpenGLData m_bunny;
	OpenGLData m_screenspace_Quad;
	OpenGLData m_light_Cube;
	// --------------------------------

	// Gen a g-buffer.
	unsigned int m_gBuffer_FBO;
	unsigned int m_albedo_Texture;
	unsigned int m_position_Texture;
	unsigned int m_normals_Texture;
	unsigned int m_gBuffer_Depth;
	// --------------------------------

	// Render lights.
	unsigned int m_light_FBO;
	unsigned int m_light_Texture;
	// --------------------------------

	// Shaders.
	unsigned int m_gBuffer_Program;
	unsigned int m_dir_Light_Program;
	unsigned int m_point_Light_Program;
	unsigned int m_spot_Light_Program;
	unsigned int m_composite_Program;
	// --------------------------------

	// Composite pass.
	// --------------------------------

	TwBar* m_bar;

private:

	Camera* m_camera;

	float m_timer;
	float dt;

};

#endif // !DEFERRED_RENDERING_H