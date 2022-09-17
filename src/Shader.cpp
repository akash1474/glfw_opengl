#include "Shader.h"

#include <fstream>
#include <iostream>
#include "vector"
#include <string>


std::string Shader::ReadFileAsString(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize((size_t)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else
	{
		printf("Could not open file %s", filepath.c_str());
	}

	return result;
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

GLuint Shader::CompileShader(GLenum type, const std::string& source)
{
	GLuint shader = glCreateShader(type);

	const GLchar* sourceCStr = source.c_str();
	glShaderSource(shader, 1, &sourceCStr, 0);

	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shader);

		printf("%s", infoLog.data());
		// HZ_CORE_ASSERT(false, "Shader compilation failure!");
	}

	return shader;
}

void Shader::activate(){
	glUseProgram(GetRendererID());
}

Shader* Shader::FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	Shader* shader = new Shader();
	shader->LoadFromGLSLTextFiles(vertexShaderPath, fragmentShaderPath);
	return shader;
}

void Shader::LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::string vertexSource = ReadFileAsString(vertexShaderPath);
	std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

	GLuint program = glCreateProgram();
	int glShaderIDIndex = 0;
		
	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	glAttachShader(program, vertexShader);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(program);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		printf("%s", infoLog.data());
	}
	
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_RendererID = program;
}
