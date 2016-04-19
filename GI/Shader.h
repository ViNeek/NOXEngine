#pragma once

#include <string>
#include <iostream>
#include <ctime>

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
    GLenum				ShaderType(){ return m_Type; }
    std::time_t			LastMod(){ return m_LastModification; }
    void                SetLastMod(std::time_t p_Tick){ m_LastModification = p_Tick; }

	// Resource Interface
	void Manage(nxShader shader) {
		std::cout << "shader load" << std::endl;
	}

	void Save() {
		std::cout << "shader save" << std::endl;
	}

	void Get() {
		std::cout << "shader get" << std::endl;
	}

	void Reload() {
		std::cout << "shader reload" << std::endl;
	}

	bool Changed() {
		std::cout << "shader changed" << std::endl;

        return true;
	}

private:

	GLuint				m_ShaderID;
	std::string			m_SourceData;
	std::string			m_FileName;
	GLenum				m_Type;
    std::time_t         m_LastModification;

};