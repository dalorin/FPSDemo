#include "TexturedLandscape.h"
#include <vector>
#include "Shader.h"
#include "Utils.h"

TexturedLandscape::TexturedLandscape(GameEngine* engine) : Object(engine)
{
	std::vector<GLint> vertices;
	vertices.push_back(1); vertices.push_back(0); vertices.push_back(-1);
	vertices.push_back(-1); vertices.push_back(0); vertices.push_back(-1);
	vertices.push_back(-1); vertices.push_back(0); vertices.push_back(1);
	vertices.push_back(1); vertices.push_back(0); vertices.push_back(1);
	
	std::vector<GLshort> texCoords;		
	texCoords.push_back(10); texCoords.push_back(10);
	texCoords.push_back(0); texCoords.push_back(10);
	texCoords.push_back(0); texCoords.push_back(0);
	texCoords.push_back(10); texCoords.push_back(0);

	std::vector<GLfloat> normals;
	for (int i = 0; i < 4; i++)
		normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);

	/*std::vector<GLint> vertices;
	for (int i = -1000; i <= 1000; i += 100)
	{	
		for (int j = -1000; j <= 1000; j += 100)
		{
			vertices.push_back(i+100); vertices.push_back(0); vertices.push_back(j);
			vertices.push_back(i); vertices.push_back(0); vertices.push_back(j);
			vertices.push_back(i); vertices.push_back(0); vertices.push_back(j+100);
			vertices.push_back(i+100); vertices.push_back(0); vertices.push_back(j+100);
		}
	}

	std::vector<GLshort> texCoords;
	for (int i = 0; i <= 441; i++)
	{	
		texCoords.push_back(1); texCoords.push_back(1);
		texCoords.push_back(0); texCoords.push_back(1);
		texCoords.push_back(0); texCoords.push_back(0);
		texCoords.push_back(1); texCoords.push_back(0);
	}

	std::vector<GLfloat> normals;
	for (int i = 0; i <= 441; i++)
	{
		normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);
	}*/

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLint), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(GLshort), &texCoords[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

	Utils::loadTexture("asphalt.tga", m_texture);

	//Set material properties.	
	m_materialProps.setProperty(MaterialProps::AMBIENT, 1.0f, 1.0f, 1.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::DIFFUSE, 1.0f, 1.0f, 1.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::SPECULAR, 0.0f, 0.0f, 0.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	m_materialProps.setShininess(0.0f);	
}

void TexturedLandscape::onRender(ShaderProgram *program)
{
	//Render content of vertex buffer
	glPushMatrix();	

	glScalef(1000.0f, 1000.0f, 1000.0f);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	program->sendMatrices();
	program->sendUniform("texture0", 0);
	program->sendMaterialProps(m_materialProps);
	
	glDrawArrays(GL_QUADS, 0, 4);
		
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glPopMatrix();
}

TexturedLandscape::~TexturedLandscape(void)
{
}
