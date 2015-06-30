#pragma once

#include <vector>

#include "CustomTypes.h"

#include <boost/thread/mutex.hpp>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class nxEntity;
class nxEngine;
class nxArcballCamera;

class nxScene {
public:

								nxScene(nxEngine* eng);
								nxScene(std::string& path);

	void						Init();
	void						PushEntity(nxEntity* ent);
	const std::string&			Filename(){ return m_SceneFilename; }
	void						SetFilename(std::string& path) { m_SceneFilename = path; }

	glm::mat4&					Projection(){ return m_PMatrix; };
	glm::mat4&					View(){ return m_VMatrix; };
	glm::mat4&					Normal(){ return glm::mat4(); };
	glm::mat4&					Rotation(){ return m_RMatrix; };
	glm::mat4&					Modelview(){ return m_MMatrix; };

	void						SetProjection(float angle, float fov, float zNear, float zFar);
	
private:

	int							m_EntitiesCount;
	std::string					m_SceneName;
	std::string					m_SceneFilename;

	std::vector<nxByte>			m_SceneData;
	std::vector<nxEntity*>		m_Entities;
	boost::mutex				m_SceneGuard;

	nxArcballCamera*			m_Camera;

	glm::mat4					m_VMatrix;
	glm::mat3					m_NMatrix;
	glm::mat4					m_RMatrix;
	glm::mat4					m_MMatrix;
	glm::mat4					m_PMatrix;

	nxEngine*					m_pEngine;
};