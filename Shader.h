#pragma once

#include "glee.h"
#include <map>

class MaterialProps;

class Shader
{
public:
	Shader(GLenum type, const char* filename);
	void load(const char* filename);
	GLuint getId();
	~Shader(void);

private:
	GLuint m_id;
};

class ShaderProgram
{
public:
	ShaderProgram(const char* vertShaderFilename, const char* fragShaderFilename);
	GLuint getId();
	void link();
	void bind();
	
	GLuint getUniform(const char* uniform);
	void bindAttrib(GLuint index, const char* attribute);
	void sendMatrices();
	void sendUniform(const char* uniform, GLint value);
	void sendUniform(const char* uniform, GLfloat value);
	void sendUniform(const char* uniform, GLfloat x, GLfloat y, GLfloat z, GLfloat a);
	void sendMaterialProps(MaterialProps props);

private:
	GLuint m_id;
	Shader *m_vertexShader;
	Shader *m_fragmentShader;
	std::map<const GLchar*, GLuint> m_uniforms;
};