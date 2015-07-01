#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

class nxShader;

class nxProgram {

public:

											nxProgram(int initCount);

	void									ProgramLog();
	bool									ReadyForLinking() { return m_InitShaderCount == m_Shaders.size(); }
	
	void									Attach();
	void									Link();
	void									Unlink();
	void									Destroy();
	void									Create();

	bool									IsLinked() { return m_Linked; };
	void									AddShader(nxShader* shader) { m_Shaders.push_back(shader); }

	void									UseProgram() { glUseProgram(m_ProgramID); };
	void									SetUniform(const char *name, float x, float y, float z);
	void									SetUniform(const char *name, const glm::vec3 & v);
	void									SetUniform(const char *name, const glm::vec4 & v);
	void									SetUniform(const char *name, const glm::ivec4 & v);
	void									SetUniform(const char *name, const glm::ivec3 & v);
	void									SetUniform(const char *name, const glm::mat4 & m);
	void									SetUniform(const char *name, const glm::mat3 & m);
	void									SetUniform(const char *name, float val);
	void									SetUniform(const char *name, int val);
	void									SetUniform(const char *name, bool val);
	void									ShowActiveUniforms();
	void									ShowActiveAttributes();

private:

	GLuint									m_ProgramID;

	bool									m_Linked;
	std::vector<nxShader*>					m_Shaders;

	size_t									m_InitShaderCount;

	int										GetUniformLocation(const char* name);
};