#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

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

private:

	GLuint									m_ProgramID;

	bool									m_Linked;
	std::vector<nxShader*>					m_Shaders;

	size_t									m_InitShaderCount;
};