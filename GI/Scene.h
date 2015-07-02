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

struct nxMatrixState {
	glm::mat4					m_VMatrix;
	glm::mat4					m_RMatrix;
	glm::mat4					m_MMatrix;
	glm::mat4					m_PMatrix;
};

class nxScene{
public:

								nxScene(nxEngine* eng);
								nxScene(std::string& path);

	void*						operator new(size_t i)
	{
		//void *p = _mm_malloc(i, 16);
		void *p = boost::alignment::aligned_alloc(16, i);
		assert(boost::alignment::is_aligned(16, p));
		return p;
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void						Init();
	const std::string&			Filename(){ return m_SceneFilename; }
	void						SetFilename(std::string path) { m_SceneFilename = path; }

	glm::mat4					Projection(){ return m_MState.m_PMatrix; };
	glm::mat4					View(){ return m_MState.m_VMatrix; };
	glm::mat4					Normal();
	glm::mat4					Rotation(){ return m_MState.m_RMatrix; };
	glm::mat4					Modelview(){ return m_MState.m_MMatrix; };

	void						SetProjection(float angle, float fov, float zNear, float zFar);
	
	void						AddEntity(nxEntity* ent) { m_Entities.push_back(ent); }

	void						Draw();

private:

	__declspec(align(16)) 
	nxMatrixState				m_MState;

	int							m_EntitiesCount;
	std::string					m_SceneName;
	std::string					m_SceneFilename;

	std::vector<nxByte>			m_SceneData;
	std::vector<nxEntity*>		m_Entities;
	boost::mutex				m_SceneGuard;

	nxArcballCamera*			m_Camera;

	nxEngine*					m_pEngine;
};

