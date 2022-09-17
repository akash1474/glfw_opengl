#pragma once

#include <string>
#include "GL/glew.h"

class Shader
{
public:
	~Shader();
	GLuint GetRendererID() { return m_RendererID; }
	static Shader* FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	static std::string ReadFileAsString(const std::string& filepath);
	Shader() = default;
	void activate();
private:
	void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	GLuint CompileShader(GLenum type, const std::string& source);
	GLuint m_RendererID;
};
