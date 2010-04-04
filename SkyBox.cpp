#include "SkyBox.h"
#include "Utils.h"
#include <vector>
#include "Shader.h"
#include "GameEngine.h"

SkyBox::SkyBox(GameEngine *engine, 
			   const char* north, 
			   const char* south, 
			   const char* east, 
			   const char* west, 
			   const char* up, 
			   const char* down)
			   : Object(engine)
{
	// Load skybox textures
	m_textures = new GLuint[5];
	glGenTextures(5, m_textures);
	Utils::loadTexture(north, m_textures[0]);
	Utils::loadTexture(south, m_textures[1]);
	Utils::loadTexture(east, m_textures[2]);
	Utils::loadTexture(west, m_textures[3]);
	Utils::loadTexture(up, m_textures[4]);

	GLfloat length, neg_length;
	length = 1.0f;
	neg_length = -length;
	// Construct skybox
	GLfloat vertices[] = {
		//North
		length, length, neg_length,
		neg_length, length, neg_length,
		neg_length, neg_length, neg_length,
		length, neg_length, neg_length,
	
		//South
		neg_length, length, length,
		length, length, length,
		length, neg_length, length,
		neg_length, neg_length, length,
		
		//East
		length, length, length,
		length, length, neg_length,
		length, neg_length, neg_length,
		length, neg_length, length,
		
		//West
		neg_length, length, neg_length,
		neg_length, length, length,
		neg_length, neg_length, length,
		neg_length, neg_length, neg_length,
		
		//Up				
		neg_length, length, neg_length,
		length, length, neg_length,
		length, length, length,
		neg_length, length, length,		
	};

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 60 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	std::vector<GLshort> texCoords;
	for (int i = 0; i < 5; i++)
	{
		texCoords.push_back(1); texCoords.push_back(1);
		texCoords.push_back(0); texCoords.push_back(1);
		texCoords.push_back(0); texCoords.push_back(0);
		texCoords.push_back(1); texCoords.push_back(0);
	}

	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(GLshort), &texCoords[0], GL_STATIC_DRAW);

	//Set material properties.	
	m_materialProps.setProperty(MaterialProps::AMBIENT, 1.0f, 1.0f, 1.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::DIFFUSE, 0.0f, 0.0f, 0.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::SPECULAR, 0.0f, 0.0f, 0.0f, 1.0f);
	m_materialProps.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	m_materialProps.setShininess(0.0f);	
}

void SkyBox::onPrepare(GLfloat dt)
{
}

void SkyBox::onRender()
{
	glPushMatrix();
	
	GLfloat modelviewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
	// Remove translation from modelview matrix.
	for (int i = 12; i <= 14; i++)
		modelviewMatrix[i] = 0.0f;

	glLoadMatrixf(modelviewMatrix);	
	glScalef(500.0f, 500.0f, 500.0f);
	glTranslatef(0.0f, 0.2f, 0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glVertexAttribPointer(2, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);
	
	ShaderProgram *program = m_engine->m_skyboxProgram;
	program->bind();

	program->sendMatrices();
	program->sendUniform("texture0", 0);	
	program->sendMaterialProps(m_materialProps);
	
	for (int i = 0; i < 5; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
		glDrawArrays(GL_QUADS, (i) * 4, 4);
	}

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(0);

	glPopMatrix();
}

SkyBox::~SkyBox(void)
{
}
