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

	m_collider = NULL;

	m_alive = true;
	m_can_delete = false;
	m_time_since_death = 0.0f;
}

Object::~Object()
{
	delete m_position;
	delete m_scale;
	delete m_velocity;
	delete m_acceleration;
	delete m_collider;
}

const char* Object::getType()
{
	return "Object";
}

void Object::onPrepare(GLfloat dt)
{	
	if (!isAlive())
	{
		m_time_since_death += dt;
		if (m_time_since_death >= 10.0f)
			setDelete(true);
	}
	else
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
			setAlive(false);
			setDelete(true);
		}
	}
}

void Object::onRender()
{
	glPushMatrix();		
	glTranslatef(this->getPosition().x, this->getPosition().y, this->getPosition().z);
	glRotatef(this->getPitch(), 1.0f, 0.0f, 0.0f);
	glRotatef(this->getYaw(), 0.0f, 1.0f, 0.0f);
	glScalef(this->getScale().x, this->getScale().y, this->getScale().z);
}

void Object::onPostRender()
{
	glPopMatrix();
}

void Object::drawBoundingSphere()
{
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

void Object::setVelocity(Vector3 velocity)
{
	*m_velocity = velocity;
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

void Object::setAcceleration(Vector3 acceleration)
{
	*m_acceleration = acceleration;
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

/**
 * setPitch
 * Sets pitch to the given angle.
 * WARNING: Use adjustPitch wherever possible as this method does not limit pitch.
 **/
void Object::setPitch(GLfloat angle)
{
	m_pitch = angle;
}

/**
 * setYaw
 * Sets yaw to the given angle.
 * WARNING: Use adjustYaw wherever possible as this method does not limit yaw.
 **/
void Object::setYaw(GLfloat angle)
{
	m_yaw = angle;
}

void Object::kill()
{
	m_alive = false;
}

Box* Object::getCollider()
{
	//TODO implement default collider
	if (!m_collider)
	{
		m_collider = new Box(NULL, 5.0f);
	}

	return m_collider;
}

/** 
 * getVertices
 * Build and return a vector containing the object's vertices.
 **/
std::vector<Vector3*> Box::getVertices(Vector3 translation)
{
	std::vector<Vector3*> vertices(8);
	vertices[0] = new Vector3(m_x2 + translation.x, m_y2 + translation.y, m_z2 + translation.z);
	vertices[1] = new Vector3(m_x1 + translation.x, m_y2 + translation.y, m_z2 + translation.z);
	vertices[2] = new Vector3(m_x1 + translation.x, m_y1 + translation.y, m_z2 + translation.z);
	vertices[3] = new Vector3(m_x2 + translation.x, m_y1 + translation.y, m_z2 + translation.z);	
	vertices[4] = new Vector3(m_x2 + translation.x, m_y2 + translation.y, m_z1 + translation.z);
	vertices[5] = new Vector3(m_x2 + translation.x, m_y1 + translation.y, m_z1 + translation.z);
	vertices[6] = new Vector3(m_x1 + translation.x, m_y1 + translation.y, m_z1 + translation.z);
	vertices[7] = new Vector3(m_x1 + translation.x, m_y2 + translation.y, m_z1 + translation.z);

	return vertices;
}

Box::Box(GameEngine* engine, GLfloat length)
: Object(engine)
{
	GLfloat hl = length / 2.0f;
	initBox(-hl, hl, -hl, hl, -hl, hl);
}

Box::Box(GameEngine* engine, GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2)
: Object(engine)
{
	initBox(x1, x2, y1, y2, z1, z2);
}

void Box::initBox(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2)
{
	m_x1 = x1;
	m_y1 = y1;
	m_z1 = z1;
	m_x2 = x2;
	m_y2 = y2;
	m_z2 = z2;

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

	m_colors.resize(96);
	for (int i = 0; i < 96; ++i)
		m_colors[i] = colorArray[i];

	//Front
	GLfloat vertexArray[] = {
		//Front
		m_x2, m_y2, m_z2,
		m_x1, m_y2, m_z2,
		m_x1, m_y1, m_z2,
		m_x2, m_y1, m_z2,
	
		//Back
		m_x2, m_y2, m_z1,
		m_x2, m_y1, m_z1,
		m_x1, m_y1, m_z1,
		m_x1, m_y2, m_z1,
		
		//Left
		m_x1, m_y2, m_z2,
		m_x1, m_y2, m_z1,
		m_x1, m_y1, m_z1,
		m_x1, m_y1, m_z2,
		
		//Right
		m_x2, m_y2, m_z1,
		m_x2, m_y2, m_z2,
		m_x2, m_y1, m_z2,
		m_x2, m_y1, m_z1,
		
		//Top
		m_x2, m_y2, m_z1,
		m_x1, m_y2, m_z1,
		m_x1, m_y2, m_z2,
		m_x2, m_y2, m_z2,
		
		//Bottom
		m_x2, m_y1, m_z2,
		m_x1, m_y1, m_z2,
		m_x1, m_y1, m_z1,
		m_x2, m_y1, m_z1,
	};

	m_vertices.resize(72);
	for (int i = 0; i < 72; ++i)
		m_vertices[i] = vertexArray[i];

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