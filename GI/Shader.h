#pragma once

#include <string>

#include <GL/glew.h>

class nxShader {

public:

						nxShader();
						nxShader(std::string& path, GLenum shaderType);

	void				LoadFromFile(std::string& path, GLenum shaderType);
	void				ShaderLog();

private:

	GLuint				m_ShaderID;
	std::string			m_SourceData;
};