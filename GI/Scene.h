#pragma once

#include <vector>

#include "CustomTypes.h"

#include <boost/thread/mutex.hpp>
#include <boost/align/align.hpp>
#include <boost/align/aligned_alloc.hpp>
#include <boost/align/is_aligned.hpp>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class nxEntity;
class nxEngine;
class nxArcballCamera;
class nxVoxelizer;
class nxLight;

struct nxMatrixState {
	glm::mat4					m_VMatrix;
	glm::mat4					m_RMatrix;
	glm::mat4					m_MMatrix;
	glm::mat4					m_PMatrix;
	glm::mat3					m_NMatrix;
};

class nxScene{
public:

								nxScene(nxEngine* eng);
								nxScene(std::string& path);

		/*
		aligned new for use with GLM
		*/
	void*						operator new(size_t i)
	{
		//void *p = _mm_malloc(i, 16);
		void *p = boost::alignment::aligned_alloc(16, i);
		assert(boost::alignment::is_aligned(16, p));
		return p;
	}

		/*
		aligned new for use with GLM
		*/
	void						operator delete(void* p)
	{
		boost::alignment::aligned_free(p);
		//_mm_free(p);
	}

	void						Init();
	const std::string&			Filename(){ return m_SceneFilename; }
	void						SetFilename(std::string path) { m_SceneFilename = path; }

	glm::mat4&					Projection(){ return m_MState.m_PMatrix; };
	glm::mat4&					View(){ return m_MState.m_VMatrix; };
	glm::mat3&					Normal();
	glm::mat4&					Rotation(){ return m_MState.m_RMatrix; };
	glm::mat4&					Modelview(){ return m_MState.m_MMatrix; };

	void						SetProjection(float angle, float fov, float zNear, float zFar);
	void						SetProjection(glm::mat4& proj);

	void						AddEntity(nxEntity* ent) { m_Entities.push_back(ent); }
	void						AddDebugEntity(nxEntity* ent) { m_DebugEntities.push_back(ent); }

	void						Draw();
	void						DrawVoxelized();

	bool						CameraReady() { return m_Camera != NULL; }

    nxArcballCamera*			Camera() { return m_Camera; }
    void            			CaptureRSM();

	void						UpdateBounds(nxEntity* ent);
	float						GMaxX(){ return m_GMaxX; }
	float						GMaxY(){ return m_GMaxY; }
	float						GMaxZ(){ return m_GMaxZ; }
	float						GMinX(){ return m_GMinX; }
	float						GMinY(){ return m_GMinY; }
	float						GMinZ(){ return m_GMinZ; }

private:

	__declspec(align(16)) 
	nxMatrixState				m_MState;

	int							m_EntitiesCount;
	std::string					m_SceneName;
	std::string					m_SceneFilename;

	std::vector<nxByte>			m_SceneData;
	std::vector<nxEntity*>		m_Entities;
	std::vector<nxEntity*>		m_DebugEntities;
	std::vector<nxLight*>		m_Lights;
	boost::mutex				m_SceneGuard;

	nxArcballCamera*			m_Camera;

	nxEngine*					m_pEngine;

	float						m_GMaxX, m_GMaxY, m_GMaxZ;
	float						m_GMinX, m_GMinY, m_GMinZ;

};

