#include "Shader.h"

#include "JobFactory.h"

#include <fstream>
#include <string>
#include "Engine.h"
#include "Renderer.h"
#include "Program.h"

#include <boost/log/trivial.hpp>

nxShader::nxShader() {
	m_ShaderID = -1;
}

nxShader::nxShader(std::string& path, GLenum shaderType) {
	m_ShaderID = -1;
	m_Type = shaderType;

	LoadSourceFromFile(path);
}

void nxShader::Create() {
	//Create shader ID
	m_ShaderID = glCreateShader(m_Type);

	//Set shader source
	const GLchar* shaderSource = m_SourceData.c_str();
	glShaderSource(m_ShaderID, 1, (const GLchar**)&shaderSource, NULL);

	//Compile shader source
	glCompileShader(m_ShaderID);

	//Check shader for errors
	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &shaderCompiled);
	if (shaderCompiled != GL_TRUE)
	{
		BOOST_LOG_TRIVIAL(error) << "Uncompilable shader: ";
		ShaderLog();
		glDeleteShader(m_ShaderID);
		m_ShaderID = -1;
	}
}

void nxShader::LoadSourceFromFile(std::string& path) {
	std::ifstream	sourceFile(path.c_str());

	if (sourceFile)
	{
		//Get shader source
		m_SourceData.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());
		BOOST_LOG_TRIVIAL(info) << "Shader source: " << m_SourceData ;
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Invalid shader file : " << path;
	}
}

void nxShader::ShaderLog()
{
	//Make sure name is shader
	if (glIsShader(m_ShaderID))
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetShaderInfoLog(m_ShaderID, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			BOOST_LOG_TRIVIAL(error) << "Shader Log : " << infoLog;
		}

		delete[] infoLog;
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Shader "<< m_ShaderID <<" : not a shader ";
	}
}


bool nxShaderLoader::operator()(void* data) {
	nxShaderLoaderBlob* blob = (nxShaderLoaderBlob*)data;

	nxShader* newShader = new nxShader(blob->m_Source, blob->m_Type);

	nxShaderCompilerBlob* newData =
		new nxShaderCompilerBlob(blob->m_Engine, blob->m_Prog, newShader);

	blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_COMPILE_SHADER, newData));
	
	return true;
}

bool nxShaderCompiler::operator()(void* data) {
	nxShaderCompilerBlob* blob = (nxShaderCompilerBlob*)data;

	blob->m_Shader->Create();

	blob->m_Prog->AddShader(blob->m_Shader);

	if (blob->m_Prog->ReadyForLinking()) {
		std::cout << "Ready for linking" << std::endl;
		nxProgramLinkerBlob* newData =
			new nxProgramLinkerBlob(blob->m_Engine, blob->m_Prog);
		blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LINK_PROGRAM, newData));

	}

	return true;
};
