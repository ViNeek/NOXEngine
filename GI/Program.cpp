#include "Program.h"

#include "Job.h"
#include "JobFactory.h"
#include "Shader.h"

#include "GLUtils.h"

#include <iostream>

nxProgram::nxProgram(int initCount) {
	m_InitShaderCount = initCount;
	m_ProgramID = -1;
	m_Linked = false;
}

void nxProgram::ProgramLog() {
	int infologLen = 0;
	int charsWritten = 0;
	GLchar* infoLog = NULL;

	glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 0)
	{

		infoLog = new GLchar[infologLen];
		if (infoLog == NULL)
		{
			printf("ERROR: Could not allocate InfoLog buffer\n");

			return;
		}
		glGetProgramInfoLog(m_ProgramID, infologLen, &charsWritten, infoLog);
		printf("\nProgram InfoLog:\n%s\n\n", infoLog);
		delete infoLog;
		infoLog = NULL;
	}
}

void nxProgram::Create() {
	m_ProgramID = glCreateProgram();
}

void nxProgram::Link() {
	GLint linkStatus;

	glLinkProgram(m_ProgramID);
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus)
		ProgramLog();

	m_Linked = true;

}

void nxProgram::Unlink() {

}

void nxProgram::Destroy() {

}

void nxProgram::Attach() {

	std::cout << "Program ID " << m_ProgramID;
	for (size_t i = 0; i < m_Shaders.size(); i++) {
		std::cout << "Shader ID " << m_Shaders[i]->ShaderID();
		glAttachShader(m_ProgramID, m_Shaders[i]->ShaderID());
	}

}

bool nxProgramLinker::operator()(void* data) {
	nxProgramLinkerBlob* blob = (nxProgramLinkerBlob*)data;

	blob->m_Prog->Create();
	Utils::GL::CheckGLState("Creating");
	blob->m_Prog->Attach();
	Utils::GL::CheckGLState("Attaching");
	blob->m_Prog->Link();
	Utils::GL::CheckGLState("Linking");

	std::cout << "Program Linked " << blob->m_Prog->IsLinked();

	return true;
};