#pragma once

// STD print
#include <iostream>

// STD Vector and String
#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

#include "CustomTypes.h"

class nxShader;

class nxProgram {

public:

											nxProgram(int initCount);
											nxProgram(nxProgram&& prog) {
												std::cout << "Move it Program" << std::endl;
											}

	void									ProgramLog();
	bool									ReadyForLinking() { return m_InitShaderCount == m_Shaders.size(); }
	bool									IsDefault() { return m_InitShaderCount == 0; }

	void									Attach();
	void									Link();
	void									Unlink();
	void									Destroy();
	void									Create();
	void									Use() { glUseProgram(m_ProgramID); }

	bool									IsLinked() { return m_Linked; };
	void									AddShader(nxShader* shader) { m_Shaders.push_back(shader); }

	void									UseProgram() { glUseProgram(m_ProgramID); };
	void									SetUniform(const char *name, float x, float y, float z);
	void									SetUniform(const char *name, const glm::vec3 & v);
	void									SetUniform(const char *name, const glm::vec4 & v);
	void									SetUniform(const char *name, const glm::ivec4 & v);
	void									SetUniform(const char *name, const glm::ivec3 & v);
	void									SetUniform(const char *name, const glm::uvec4 & v);
	void									SetUniform(const char *name, const glm::uvec3 & v);
	void									SetUniform(const char *name, const glm::mat4 & m);
	void									SetUniform(const char *name, int count, const glm::mat4 * m);
	void									SetUniform(const char *name, const glm::mat3 & m);
	void									SetUniform(const char *name, float val);
	void									SetUniform(const char *name, int val);
	void									SetUniform(const char *name, nxUInt32 val);
	void									SetUniform(const char *name, bool val);
	void									ShowActiveUniforms();
	void									ShowActiveAttributes();

	void									SetName(std::string& name) { m_ProgramName = name; };
	const std::string&						GetName() { return m_ProgramName; };
	GLuint									GetID() { return m_ProgramID; };

	// Resource Interface
	void Manage(nxProgram program) {
		std::cout << "manage" << std::endl;
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

	GLuint									m_ProgramID;

	bool									m_Linked;
	std::vector<nxShader*>					m_Shaders;

	std::string								m_ProgramName;

	size_t									m_InitShaderCount;

	int										GetUniformLocation(const char* name);
};