#include "objects.h"
#include <iostream>
#include <sstream>
#include "Shader.h"
#include "targa.h"
#include "GameEngine.h"
#include "Utils.h"

Object::Object(GameEngine* engine)
{
	m_position = new Vector3();
	m_scale = new Vector3(1.0f, 1.0f, 1.0f);
	m_velocity = new Vector3();
	m_acceleration = new Vector3();

	m_pitch = 0.0f;
	m_yaw = 0.0f;

	m_engine = engine;

	m_alive = true;
}

Object::~Object()
{
	delete m_position;
	delete m_scale;
	delete m_velocity;
	delete m_acceleration;
}

void Object::onPrepare(GLfloat dt)
{	
	this->setVelocity(
			this->getVelocity().x + this->getAcceleration().x,
			this->getVelocity().y + this->getAcceleration().y,
			this->getVelocity().z + this->getAcceleration().z
		);
	this->setPosition(
			this->getPosition().x + this->getVelocity().x,
			this->getPosition().y + this->getVelocity().y,
			this->getPosition().z + this->getVelocity().z
		);	
	//Check if object has left the game area
	if (this->getPosition().x < -1000.0f || this->getPosition().x > 1000.0f ||
		this->getPosition().y < -10.0f || this->getPosition().y > 1000.0f ||
		this->getPosition().z < -1000.0f || this->getPosition().z > 1000.0f)
	{
		m_alive = false;
	}
}

void Object::onRender()
{
	glPushMatrix();	
	glScalef(this->getScale().x, this->getScale().y, this->getScale().z);
	glTranslatef(this->getPosition().x, this->getPosition().y, this->getPosition().z);
	glRotatef(this->getYaw(), 0.0f, 1.0f, 0.0f);
	glRotatef(this->getPitch(), 1.0f, 0.0f, 0.0f);
}

void Object::onPostRender()
{
	glPopMatrix();
}

void Object::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	m_position->x = x;
	m_position->y = y;
	m_position->z = z;
}

void Object::setPosition(Vector3 position)
{
	*m_position = position;
}

Vector3 Object::getPosition()
{
	return *m_position;
}

void Object::setScale(GLfloat x, GLfloat y, GLfloat z)
{
	m_scale->x = x;
	m_scale->y = y;
	m_scale->z = z;
}

Vector3 Object::getScale()
{
	return *m_scale;
}

void Object::setVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	m_velocity->x = x;
	m_velocity->y = y;
	m_velocity->z = z;
}

Vector3 Object::getVelocity()
{
	return *m_velocity;
}

void Object::setAcceleration(GLfloat x, GLfloat y, GLfloat z)
{
	m_acceleration->x = x;
	m_acceleration->y = y;
	m_acceleration->z = z;
}

Vector3 Object::getAcceleration()
{
	return *m_acceleration;
}

void Object::adjustPitch(GLfloat angle)
{
	m_pitch += angle;

	if (m_pitch > 90.0f)
		m_pitch = 90.0f;
	else if (m_pitch < -90.0f)
		m_pitch = -90.0f;
}

void Object::adjustYaw(GLfloat angle)
{
	m_yaw += angle;
	
	if (m_yaw >= 360.0f)
		m_yaw -= 360.0f;
	else if (m_yaw < 0.0f)
		m_yaw += 360.0f;
}

Box::Box(GameEngine* engine, GLfloat length) : Object(engine)
{
	this->length = length;
	m_rotation = 0.0f;

	GLfloat neg_length = -1.0f * length;
	
	GLdouble colorArray[] = {
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
	};

	for (int i = 0; i < 96; ++i)
		m_colors.push_back(colorArray[i]);

	//Front
	GLfloat vertexArray[] = {
		//Front
		length, length, length,
		neg_length, length, length,
		neg_length, neg_length, length,
		length, neg_length, length,
	
		//Back
		length, length, neg_length,
		length, neg_length, neg_length,
		neg_length, neg_length, neg_length,
		neg_length, length, neg_length,
		
		//Left
		neg_length, length, length,
		neg_length, length, neg_length,
		neg_length, neg_length, neg_length,
		neg_length, neg_length, length,
		
		//Right
		length, length, neg_length,
		length, length, length,
		length, neg_length, length,
		length, neg_length, neg_length,
		
		//Top
		length, length, neg_length,
		neg_length, length, neg_length,
		neg_length, length, length,
		length, length, length,
		
		//Bottom
		length, neg_length, length,
		neg_length, neg_length, length,
		neg_length, neg_length, neg_length,
		length, neg_length, neg_length,
	};

	for (int i = 0; i < 72; ++i)
		m_vertices.push_back(vertexArray[i]);

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLdouble) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);
}

void Box::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);

	/*m_rotation += dt * 250.0f;
	if (m_rotation >= 360.0f)
		m_rotation = 0.0f;*/

	/*	
	glBegin(GL_QUADS);
	//Front
	glVertex3f(length, length, length);
	glVertex3f(neg_length, length, length);
	glVertex3f(neg_length, neg_length, length);
	glVertex3f(length, neg_length, length);

	//Back
	glVertex3f(length, length, neg_length);
	glVertex3f(length, neg_length, neg_length);
	glVertex3f(neg_length, neg_length, neg_length);
	glVertex3f(neg_length, length, neg_length);
	
	//Left
	glVertex3f(neg_length, length, length);
	glVertex3f(neg_length, length, neg_length);
	glVertex3f(neg_length, neg_length, neg_length);
	glVertex3f(neg_length, neg_length, length);
	
	//Right
	glVertex3f(length, length, neg_length);
	glVertex3f(length, length, length);
	glVertex3f(length, neg_length, length);
	glVertex3f(length, neg_length, neg_length);
	
	//Top
	glVertex3f(length, length, neg_length);
	glVertex3f(neg_length, length, neg_length);
	glVertex3f(neg_length, length, length);
	glVertex3f(length, length, length);
	
	//Bottom
	glVertex3f(length, neg_length, length);
	glVertex3f(neg_length, neg_length, length);
	glVertex3f(neg_length, neg_length, neg_length);
	glVertex3f(length, neg_length, neg_length);
	glEnd();*/
		
}

void Box::onRender()
{
	Object::onRender();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);						
	
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glVertexAttribPointer(1, 4, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);	
	
	ShaderProgram *shaderProgram = m_engine->m_basicProgram;
	shaderProgram->bind();

	shaderProgram->sendMatrices();
	
	glDrawArrays(GL_QUADS, 0, 24);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	/*GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::stringstream str;
		str << "Error drawing box: error code " << err;
		MessageBox(NULL, str.str().c_str(), "OpenGL Error!", MB_ICONERROR | MB_OK);
	}*/

}

Crosshair::Crosshair(GameEngine *engine) : 
Object(engine),
m_buffer(0),
m_textureCoords(0),
m_texture(0)
{	
	GLfloat vertices[] = 
		{16.0f, 16.0f, 0.0f,
		 -16.0f, 16.0f, 0.0f,
		 -16.0f, -16.0f, 0.0f,
		 16.0f, -16.0f, 0.0f};

	GLushort textureCoords[] = 
		{1, 1,
		 0, 1,
		 0, 0,
		 1, 0};

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);	
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_textureCoords);
	glBindBuffer(GL_ARRAY_BUFFER, m_textureCoords);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLushort), &textureCoords, GL_STATIC_DRAW);
	
	Utils::loadTexture("crosshair.tga", m_texture);
}

void Crosshair::onRender()
{
	Object::onRender();

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_textureCoords);
	glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	ShaderProgram *shaderProgram = m_engine->m_hudProgram;
	shaderProgram->bind();

	shaderProgram->sendMatrices();
	shaderProgram->sendUniform("texture0", 0);

	glBindTexture(GL_TEXTURE_2D, m_texture);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}