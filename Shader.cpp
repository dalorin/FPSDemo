#include "Shader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "objects.h"

using namespace std;

Shader::Shader(GLenum type, const char* filename)
{
	// Create shader
	m_id = glCreateShader(type);
	
	// Load source into shader
	stringstream buffer;
	ifstream shaderFile(filename);
	buffer << shaderFile.rdbuf();
	string str(buffer.str());
	const GLchar* tmp = static_cast<const GLchar*>(str.c_str());
	glShaderSource(m_id, 1, &tmp, NULL);

	// Compile shader
	glCompileShader(m_id);

	// Check compilation status
	GLint status;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char log[256];
		GLint logLength;
		glGetShaderInfoLog(m_id, 256, &logLength, log);
		stringstream str;
		str << filename << ": Shader compilation failed\n: ";
		for (int i = 0; i < logLength; ++i)
			str << log[i];
		MessageBox(NULL, str.str().c_str(), "Shader Error", MB_ICONERROR | MB_OK);
		exit(-1);
	}
}

GLuint Shader::getId()
{
	return m_id;
}

Shader::~Shader(void)
{
}

ShaderProgram::ShaderProgram(const char* vertShaderFilename, const char* fragShaderFilename)
{
	// Create program and shaders.
	m_id = glCreateProgram();
	m_vertexShader = new Shader(GL_VERTEX_SHADER, vertShaderFilename);
	m_fragmentShader = new Shader(GL_FRAGMENT_SHADER, fragShaderFilename);

	// Attach shaders to program.
	glAttachShader(m_id, m_vertexShader->getId());
	glAttachShader(m_id, m_fragmentShader->getId());
}

GLuint ShaderProgram::getId()
{
	return m_id;
}

void ShaderProgram::link()
{
	glLinkProgram(m_id);
	GLint linkStatus;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{		
		char log[256];
		GLint logLength;
		glGetProgramInfoLog(m_id, 256, &logLength, log);
		stringstream str;
		str << "Shader program link failed: ";
		for (int i = 0; i < logLength; ++i)
			str << log[i];
		MessageBox(NULL, str.str().c_str(), "Shader Error", MB_ICONERROR | MB_OK);
		exit(-1);
	};
}

void ShaderProgram::bind()
{
	glUseProgram(m_id);
}

GLuint ShaderProgram::getUniform(const char* uniform)
{
	return 1; // TODO Needs to be implemented
}

void ShaderProgram::bindAttrib(GLuint index, const char* attribute)
{
	glBindAttribLocation(m_id, index, attribute);
}

void ShaderProgram::sendMatrices()
{
	GLfloat projectionMatrix[16];
	GLfloat modelviewMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);

	GLuint projMatrixUniform = glGetUniformLocation(m_id, "projection_matrix");
	glUniformMatrix4fv(projMatrixUniform, 1, FALSE, projectionMatrix);

	GLuint mvMatrixUniform = glGetUniformLocation(m_id, "modelview_matrix");
	glUniformMatrix4fv(mvMatrixUniform, 1, FALSE, modelviewMatrix);	
}

void ShaderProgram::sendUniform(const char* uniform, GLint value)
{
	GLuint uniformLocation = glGetUniformLocation(m_id, uniform);
	glUniform1i(uniformLocation, value);
}

void ShaderProgram::sendUniform(const char* uniform, GLfloat value)
{
	GLuint uniformLocation = glGetUniformLocation(m_id, uniform);
	glUniform1f(uniformLocation, value);
}

void ShaderProgram::sendUniform(const char* uniform, GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	GLuint uniformLocation = glGetUniformLocation(m_id, uniform);
	glUniform4f(uniformLocation, x, y, z, a);
}

void ShaderProgram::sendMaterialProps(MaterialProps props)
{
	Color color;
	color = props.getProperty(MaterialProps::AMBIENT);
	this->sendUniform("material_ambient", color.r, color.g, color.b, color.a);
	color = props.getProperty(MaterialProps::DIFFUSE);
	this->sendUniform("material_diffuse", color.r, color.g, color.b, color.a);
	color = props.getProperty(MaterialProps::SPECULAR);
	this->sendUniform("material_specular", color.r, color.g, color.b, color.a);
	color = props.getProperty(MaterialProps::EMISSIVE);
	this->sendUniform("material_emissive", color.r, color.g, color.b, color.a);
	this->sendUniform("material_shininess", props.getShininess());
}