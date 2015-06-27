#include "Shader.h"

#include "JobFactory.h"

#include <fstream>
#include <string>

#include <boost/log/trivial.hpp>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include <map>

nxShader::nxShader() {
	m_ShaderID = -1;
}

nxShader::nxShader(std::string& path, GLenum shaderType) {
	m_ShaderID = -1;

	LoadFromFile(path, shaderType);
}

void nxShader::LoadFromFile(std::string& path, GLenum shaderType) {
	std::ifstream	sourceFile(path.c_str());

	if (sourceFile)
	{
		//Get shader source
		m_SourceData.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());
		
		//Create shader ID
		m_ShaderID = glCreateShader(shaderType);

		//Set shader source
		const GLchar* shaderSource = m_SourceData.c_str();
		glShaderSource(m_ShaderID, 1, (const GLchar**)&m_SourceData, NULL);

		//Compile shader source
		glCompileShader(m_ShaderID);

		//Check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE)
		{
			BOOST_LOG_TRIVIAL(error) << "Uncompilable shader: " << path;
			ShaderLog();
			glDeleteShader(m_ShaderID);
			m_ShaderID = -1;
		}
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

std::map<std::string, GLenum> gc_TypeMappings =
				boost::assign::map_list_of	("compute", GL_COMPUTE_SHADER)
											("vertex", GL_VERTEX_SHADER)
											("tess_control", GL_TESS_CONTROL_SHADER)
											("tess_evaluation", GL_TESS_EVALUATION_SHADER)
											("geometry", GL_GEOMETRY_SHADER)
											("fragment", GL_FRAGMENT_SHADER);

bool nxShaderLoader::operator()(void* data) {

	return true;
}