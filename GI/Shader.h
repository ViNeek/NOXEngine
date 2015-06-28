#pragma once

#include <string>

#include <GL/glew.h>

class nxShader {

public:

						nxShader();
						nxShader(std::string& path, GLenum shaderType);

	void				LoadSourceFromFile(std::string& path);
	void				ShaderLog();
	void				Create();
	GLuint				ShaderID(){ return m_ShaderID; }

private:

	GLuint				m_ShaderID;
	std::string			m_SourceData;
	GLenum				m_Type;
};