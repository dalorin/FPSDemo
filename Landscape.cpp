#include "Landscape.h"
#include <vector>
#include "Shader.h"
#include "GameEngine.h"

Landscape::Landscape(GameEngine *engine) : Object(engine)
{
	//Populate VBO with 3D grid for now
	std::vector<GLint> vertices;
	for (int i = -1000; i <= 1000; i += 10)
	{
		vertices.push_back(i);
		vertices.push_back(0);
		vertices.push_back(-1000);
		vertices.push_back(i);
		vertices.push_back(0);
		vertices.push_back(1000);
	}
	for (int i = -1000; i <= 1000; i += 10)
	{
		vertices.push_back(-1000);
		vertices.push_back(0);
		vertices.push_back(i);
		vertices.push_back(1000);
		vertices.push_back(0);
		vertices.push_back(i);
	}

	GLdouble colors[3216];
	for (int i = 0; i < 3216; i += 4)
	{
		colors[i] = 0.0;
		colors[i+1] = 0.4;
		colors[i+2] = 0.0;
		colors[i+3] = 1.0;
	}

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLint), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_color);
	glBindBuffer(GL_ARRAY_BUFFER, m_color);
	glBufferData(GL_ARRAY_BUFFER, 3216 * sizeof(GLdouble), &colors[0], GL_STATIC_DRAW);
}

void Landscape::onRender()
{
	//Render content of vertex buffer
	glPushMatrix();	

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_color);
	glVertexAttribPointer(1, 4, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	ShaderProgram *program = m_engine->m_modelProgram;
	program->bind();

	program->sendMatrices();
	
	glDrawArrays(GL_LINES, 0, 804);

	glPopMatrix();
}

Landscape::~Landscape(void)
{
}
