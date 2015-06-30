#pragma once

#include <vector>

#include "CustomTypes.h"

#include <boost/thread/mutex.hpp>

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

private:

	int							m_EntitiesCount;
	std::string					m_SceneName;
	std::string					m_SceneFilename;

	std::vector<nxByte>			m_SceneData;
	std::vector<nxEntity*>		m_Entities;
	boost::mutex				m_SceneGuard;

	nxArcballCamera*			m_Camera;

	nxEngine*					m_pEngine;
};