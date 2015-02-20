#include "Animation.h"
#include "Gizmos.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "FlyCamera.h"

#include "Uitility.h"

bool Animation::StartUp()
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

	m_file = new FBXFile();
	m_file->load("./Models/characters/Enemyelite/EnemyElite.fbx");
	m_file->initialiseOpenGLTextures();

	GenerateGLMeshes(m_file);
	LoadShader("./Shaders/Skinned_Vertex.glsl", "./Shaders/Skinned_Fragment.glsl", &m_program_ID);

	m_timer = 0;

	return true;
}

void Animation::ShutDown()
{
	m_file->unload();
	delete m_file;

	Gizmos::destroy();

	Application::ShutDown();
}

bool Animation::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_camera->Update(dt);

	dt = glfwGetTime();
	glfwSetTime(0.0);

	FBXSkeleton* skele_man = m_file->getSkeletonByIndex(0);
	FBXAnimation* anim = m_file->getAnimationByIndex(0);

	m_timer += (float)dt;

	//skele_man->evaluate(anim, m_timer);

	EvaluateSkeleton(anim, skele_man, m_timer);

	for (unsigned int i = 0; i < skele_man->m_boneCount; i++)
	{
		skele_man->m_nodes[i]->updateGlobalTransform();

		mat4 node_Global = skele_man->m_nodes[i]->m_globalTransform;
		vec3 node_Pos = node_Global[3].xyz;

		Gizmos::addAABBFilled(node_Pos, vec3(25.f), vec4(1, 0, 0, 1), &node_Global);

		if (skele_man->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_Pos = skele_man->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(node_Pos, parent_Pos, vec4(0, 1, 0, 1));
		}
	}

	return true;
}

void Animation::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionTransform, m_camera->m_viewTransform);

	glUseProgram(m_program_ID);

	int proj_View_Uniform = glGetUniformLocation(m_program_ID, "projection_view");
	int diffuse_Uniform = glGetUniformLocation(m_program_ID, "diffuse");

	glUniformMatrix4fv(proj_View_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);
	glUniform1i(diffuse_Uniform, 0);

	FBXSkeleton* skeleton = m_file->getSkeletonByIndex(0);
	//skeleton->updateBones();

	UpdateBones(skeleton);

	int bones_Uniform = glGetUniformLocation(m_program_ID, "bones");

	glUniformMatrix4fv(bones_Uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		FBXMeshNode* curr_Mesh = m_file->getMeshByIndex(i);
		FBXMaterial* mesh_Material = curr_Mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_Material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_Transform = curr_Mesh->m_globalTransform;
		int world_Uniform = glGetUniformLocation(m_program_ID, "world");
		glUniformMatrix4fv(world_Uniform, 1, GL_FALSE, (float*)&world_Transform);

		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_Count, GL_UNSIGNED_INT, 0);
	}

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Animation::GenerateGLMeshes(FBXFile* _fbx)
{
	unsigned int mesh_Count = _fbx->getMeshCount();

	m_meshes.resize(mesh_Count);

	for (unsigned int mesh_Index = 0; mesh_Index < mesh_Count; mesh_Index++)
	{
		FBXMeshNode* curr_Mesh = _fbx->getMeshByIndex(mesh_Index);

		m_meshes[mesh_Index].m_index_Count = curr_Mesh->m_indices.size();

		glGenVertexArrays(1, &m_meshes[mesh_Index].m_VAO);
		glGenBuffers(1, &m_meshes[mesh_Index].m_VBO);
		glGenBuffers(1, &m_meshes[mesh_Index].m_IBO);

		glBindVertexArray(m_meshes[mesh_Index].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[mesh_Index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex) * curr_Mesh->m_vertices.size(), curr_Mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[mesh_Index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curr_Mesh->m_indices.size(), curr_Mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Pos.
		glEnableVertexAttribArray(1); // Tex Coord.
		glEnableVertexAttribArray(2); // Bone Indices.
		glEnableVertexAttribArray(3); // Bone Weights.

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Animation::EvaluateSkeleton(FBXAnimation* _anim, FBXSkeleton* _skeleton, float _timer)
{
	float fps = 24.f;
	int current_Frame = (int)(_timer * fps);

	// Loop through all the bones.
	for (unsigned int track_Index = 0; track_Index < _anim->m_trackCount; track_Index++)
	{
		// Wrap back to the start of the track if we've gone too far.
		int track_Frame_Count = _anim->m_tracks[track_Index].m_keyframeCount;
		int track_Frame = current_Frame % track_Frame_Count;

		// Find what key frames are currently effecting the bones.
		FBXKeyFrame curr_Frame = _anim->m_tracks[track_Index].m_keyframes[track_Frame];
		FBXKeyFrame next_Frame = _anim->m_tracks[track_Index].m_keyframes[(track_Frame + 1) % track_Frame_Count];

		// Find out how far between key frames we are.
		float time_Since_Frame_Flip = _timer - (current_Frame / fps);
		float t = time_Since_Frame_Flip * fps;

		// Interpolate between those key frames to generate the matrix.
		// For the currecnt pose.
		vec3 new_Pos = mix(curr_Frame.m_translation, next_Frame.m_translation, t);
		vec3 new_Scale = mix(curr_Frame.m_scale, next_Frame.m_scale, t);
		
		quat new_Rot = slerp(curr_Frame.m_rotation, next_Frame.m_rotation, t);

		mat4 local_Transform = translate(new_Pos) * toMat4(new_Rot) * scale(new_Scale);

		// get the right bone for the given track.
		int bone_Index = _anim->m_tracks[track_Index].m_boneIndex;

		// Set the FBXNode's loacl transform to match.
		if (bone_Index < _skeleton->m_boneCount)
		{
			_skeleton->m_nodes[bone_Index]->m_localTransform = local_Transform;
		}
	}
}

void Animation::UpdateBones(FBXSkeleton* _skeleton)
{
	// Loop though the nodes in the skeleton.
	for (unsigned int bone_Index = 0; bone_Index < _skeleton->m_boneCount; bone_Index++)
	{
		//generate their global transform.
		int parent_Index = _skeleton->m_parentIndex[bone_Index];

		if (parent_Index == -1)
		{
			_skeleton->m_bones[bone_Index] = _skeleton->m_nodes[bone_Index]->m_localTransform;
		}
		else
		{
			_skeleton->m_bones[bone_Index] = _skeleton->m_bones[parent_Index] *_skeleton->m_nodes[bone_Index]->m_localTransform;
		}
	}

	for (unsigned int bone_Index = 0; bone_Index < _skeleton->m_boneCount; bone_Index++)
	{
		// Multiply the global transform by the inverse bind pose.
		_skeleton->m_bones[bone_Index] = _skeleton->m_bones[bone_Index] * _skeleton->m_bindPoses[bone_Index];
	}
}