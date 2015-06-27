#pragma once

#include <vector>

#include "CustomTypes.h"

#include <boost/thread/mutex.hpp>

class nxEntity;

class nxScene {
public:

								nxScene();
								nxScene(std::string& path);

	void						InitFromFile(std::string& path);
	void						PushEntity(nxEntity* ent);

private:

	int							m_EntitiesCount;
	std::string					m_SceneName;

	std::vector<nxByte>			m_SceneData;
	std::vector<nxEntity*>		m_Entities;
	boost::mutex				m_SceneGuard;


};