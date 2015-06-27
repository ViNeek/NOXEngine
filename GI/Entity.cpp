#include "Entity.h"

nxEntity::nxEntity() {
	m_IBOData = -1;
	m_VBOData = -1;
	m_VAOData = -1;
	m_MaterialIndex = -1;
	m_SceneIndex = -1;
}

nxEntity::nxEntity(const std::string& filename) {
	m_IBOData = -1;
	m_VBOData = -1;
	m_VAOData = -1;
	m_MaterialIndex = -1;
	m_SceneIndex = -1;

	InitFromFile(filename);
}