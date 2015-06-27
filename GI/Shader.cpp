#include "Shader.h"

#include <fstream>
#include <string>

nxShader::nxShader() {
	m_ShaderID = -1;
}

nxShader::nxShader(std::string& path) {
	m_ShaderID = -1;

	LoadFromFile(path);
}

void nxShader::LoadFromFile(std::string& path) {
	/*std::ifstream	sourceFile(path.c_str());

	if (sourceFile)
	{
		//Get shader source
		m_SourceData.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());
		//Create shader ID
		m_ShaderID = glCreateShader(shaderType);

		//Set shader source
		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

		//Compile shader source
		glCompileShader(shaderID);

		//Check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE)
		{
			printf("Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
			printShaderLog(shaderID);
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
	{
		printf("Unable to open file %s\n", path.c_str());
	}*/
}