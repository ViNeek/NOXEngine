#pragma once

#include <string>

#include <GL/glew.h>

class nxShader {

public:

						nxShader();
						nxShader(std::string& path);

	void				LoadFromFile(std::string& path);

private:

	GLuint				m_ShaderID;
	std::string			m_SourceData;
};