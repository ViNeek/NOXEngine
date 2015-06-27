#pragma once

#include <gl/glew.h>

#include <string>
#include <vector>

#include "CustomTypes.h"

class nxEntity {
public:

							nxEntity();
							nxEntity(const std::string filename);
	void					InitFromFile(const std::string& filename);
	void					ClearData();

private:

	std::vector<nxByte>		m_EntityData;

	GLuint					m_VBOData;
	GLuint					m_VAOData;
	GLuint					m_IBOData;
	unsigned int			m_Indices;
	unsigned int			m_MaterialIndex;

	unsigned int			m_SceneIndex;

};