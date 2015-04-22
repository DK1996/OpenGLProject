#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "Application.h"
#include "Camera.h"			
#include <FBXFile.h>
#include "Vertex.h"

class Animation : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void GenerateGLMeshes(FBXFile* _fbx);

	void EvaluateSkeleton(FBXAnimation* _anim, FBXSkeleton* _skeleton, float _timer);
	void UpdateBones(FBXSkeleton* _skeleton);

	double dt;
	float m_timer;

	unsigned int m_program_ID;

	std::vector<OpenGLData> m_meshes;

	Camera* m_camera;

	FBXFile* m_file;
};

#endif