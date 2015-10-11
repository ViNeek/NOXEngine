#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>

class nxShader {

public:

						nxShader();
						nxShader(std::string& path, GLenum shaderType);
						nxShader(nxShader&& shader);

	void				LoadSourceFromFile(std::string& path);
	void				ShaderLog();
	void				Create();
	const std::string&	Filename(){ return m_FileName; }
	void				Filename(const std::string& str){ m_FileName = str; }
	GLuint				ShaderID(){ return m_ShaderID; }

	// Resource Interface
	void Manage(nxShader shader) {
		std::cout << "load" << std::endl;
	}

	void Save() {
		std::cout << "save" << std::endl;
	}

	void Get() {
		std::cout << "get" << std::endl;
	}

	void Reload() {
		std::cout << "reload" << std::endl;
	}

	bool Changed() {
		std::cout << "changed" << std::endl;
	}

private:

	GLuint				m_ShaderID;
	std::string			m_SourceData;
	std::string			m_FileName;
	GLenum				m_Type;

};