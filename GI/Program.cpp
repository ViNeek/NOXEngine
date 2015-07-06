#include "Program.h"

#include "CustomEvents.h"
#include "Job.h"
#include "JobFactory.h"
#include "Shader.h"
#include "Engine.h"
#include "Renderer.h"
#include "Scheduler.h"

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


int nxProgram::GetUniformLocation(const char *name){
	GLint loc;
	loc = glGetUniformLocation(m_ProgramID, name);
	if (loc == -1)
		printf("No such uniform named \"%s\"\n", name);

	return loc;
}

void nxProgram::SetUniform(const char *name, float x, float y,
	float z){}

void nxProgram::SetUniform(const char *name, const glm::ivec4 & v)
{
	glUniform4iv(GetUniformLocation(name), 1, &v[0]);
}
void nxProgram::SetUniform(const char *name, const glm::ivec3 & v)
{
	glUniform3iv(GetUniformLocation(name), 1, &v[0]);
}
void nxProgram::SetUniform(const char *name, const glm::uvec3 & v)
{
	glUniform3uiv(GetUniformLocation(name), 1, &v[0]);
}
void nxProgram::SetUniform(const char *name, const glm::vec3 & v)
{
	glUniform3fv(GetUniformLocation(name), 1, &v[0]);
}
void nxProgram::SetUniform(const char *name, const glm::vec4 & v)
{
	glUniform4fv(GetUniformLocation(name), 1, &v[0]);
}
void nxProgram::SetUniform(const char *name, const glm::mat4 & m)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, false, &m[0][0]);
}
void nxProgram::SetUniform(const char *name, int count, const glm::mat4 * m)
{
	glUniformMatrix4fv(GetUniformLocation(name), count, false, &m[0][0][0]);
}
void nxProgram::SetUniform(const char *name, const glm::mat3 & m)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, false, &m[0][0]);
}
void nxProgram::SetUniform(const char *name, float val)
{
	glUniform1f(GetUniformLocation(name), val);
}
void nxProgram::SetUniform(const char *name, int val)
{
	glUniform1i(GetUniformLocation(name), val);
}
void nxProgram::SetUniform(const char *name, bool val)
{}

void nxProgram::ShowActiveAttributes()
{
	GLint maxLength, nAttribs;
	glGetProgramiv(m_ProgramID, GL_ACTIVE_ATTRIBUTES,
		&nAttribs);
	glGetProgramiv(m_ProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
		&maxLength);

	GLchar * name = (GLchar *)malloc(maxLength);

	GLint written, size, location;
	GLenum type;
	printf("\nLOG ! ! !   Attributes\n");
	printf("------------------------------------------------\n");
	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveAttrib(m_ProgramID, i, maxLength, &written,
			&size, &type, name);
		location = glGetAttribLocation(m_ProgramID, name);
		printf(" %-5d | %s\n", location, name);
	}

	free(name);
}

void nxProgram::ShowActiveUniforms(){
	GLint maxLength, nAttribs;
	glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS,
		&nAttribs);
	glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH,
		&maxLength);

	GLchar * name = (GLchar *)malloc(maxLength);

	GLint written, size, location;
	GLenum type;
	printf("\nLOG ! ! !   Uniforms\n");
	printf("------------------------------------------------\n");
	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveUniform(m_ProgramID, i, maxLength, &written,
			&size, &type, name);
		location = glGetUniformLocation(m_ProgramID, name);
		printf(" %-5d | %s\n", location, name);
	}

	free(name);
}

static int i = 1000;
bool nxProgramLinker::operator()(void* data) {
	nxProgramLinkerBlob* blob = (nxProgramLinkerBlob*)data;
	
	blob->m_Prog->Create();
	Utils::GL::CheckGLState("Creating");
	blob->m_Prog->Attach();
	Utils::GL::CheckGLState("Attaching");
	blob->m_Prog->Link();
	Utils::GL::CheckGLState("Linking");

	blob->m_Engine->Renderer()->AddProgram(blob->m_Prog);

	blob->m_Prog->ShowActiveUniforms();
	blob->m_Prog->ShowActiveAttributes();

	std::cout << "Program Linked " << blob->m_Prog->IsLinked();

	wxFrame* evtHandler = blob->m_Engine->Scheduler()->EventHandler();
	wxCommandEvent* evt = new wxCommandEvent(nxPROGRAM_ADDED_EVENT); // Still keeping it simple, don't give a specific event ID
	evt->SetInt(i++);
	evt->SetString( blob->m_Prog->GetName() );
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return true;
};