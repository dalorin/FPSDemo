#include "ParticleSystem.h"
#include "Shader.h"
#include "Utils.h"
#include "GameEngine.h"
#include <sstream>

Particle::Particle(GameEngine *engine, GLuint texture, GLfloat lifetime) : 
	m_texture(texture),
	m_lifetime(lifetime),
	m_elapsed(0.0f),
	Object(engine)
{	
	m_startColor[0] = 0.1f;
	m_startColor[1] = 0.1f;
	m_startColor[2] = 0.7f;
	m_startColor[3] = 0.8f;

	m_endColor[0] = 0.7f;
	m_endColor[1] = 0.1f;
	m_endColor[2] = 0.1f;
	m_endColor[3] = 0.4f;
}

void Particle::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);

	m_elapsed += dt;	
}

bool Particle::isAlive()
{
	return m_elapsed <= m_lifetime;
}

Particle::~Particle()
{
}

Emitter::Emitter(GameEngine *engine, 
				 int maxParticles, 
				 GLfloat lifetimeMean, 
				 GLfloat lifetimeSpread, 
				 GLfloat velocitySpread, 
				 const char* textureFilename) :
	Object(engine),
	m_maxParticles(maxParticles),
	m_lifetimeMean(lifetimeMean),
	m_lifetimeSpread(lifetimeSpread),
	m_velocitySpread(velocitySpread),
	m_firstRun(true)
{
	glGenBuffers(1, &m_vertexBuffer);
	glGenBuffers(1, &m_colorBufferA);
	glGenBuffers(1, &m_colorBufferB);
	glGenBuffers(1, &m_lerpValueBuffer);
	Utils::loadTexture(textureFilename, m_texture);

	generateParticles();

	for (int i = 0; i < m_maxParticles; i++)
	{
		for (int j = 0; j < 4; j++)
			m_colors.push_back(m_particles[i]->getStartColor()[j]);
	}

	for (int i = 0; i < m_maxParticles; i++)
	{
		for (int j = 0; j < 4; j++)
			m_colors.push_back(m_particles[i]->getEndColor()[j]);
	}

	// Create and populate colour buffers.
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferA);	
	glBufferData(GL_ARRAY_BUFFER, 4 * m_maxParticles * sizeof(GLdouble), &m_colors[0], GL_STATIC_DRAW);	

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferB);
	glBufferData(GL_ARRAY_BUFFER, 4 * m_maxParticles * sizeof(GLdouble), &m_colors[m_maxParticles], GL_STATIC_DRAW);	
}

/**
 * getRandomLifetime
 * Return random lifetime value between minLifetime and maxLifetime.
 **/
GLfloat Emitter::getRandomValue(GLfloat mean, GLfloat spread)
{
	return ((2.0f * spread) * ((GLfloat)rand()/RAND_MAX)) + (mean - spread);
}

void Emitter::generateParticles()
{
	for (int i = m_particles.size(); i < m_maxParticles; i++)
	{
		Particle *particle = new Particle(m_engine, m_texture, getRandomValue(m_lifetimeMean, m_lifetimeSpread));
		particle->setPosition(this->getPosition().x, this->getPosition().y, this->getPosition().z);
		particle->setVelocity(
			getRandomValue(-0.5f * this->getVelocity().x, m_velocitySpread), 
			getRandomValue(-0.5f * this->getVelocity().y, m_velocitySpread), 
			getRandomValue(-0.5f * this->getVelocity().z, m_velocitySpread)
		);
		m_particles.push_back(particle);
	}
}

void Emitter::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);

	// Iterate backwards as we'll be removing elements from the array.
	for (int i = m_particles.size() - 1; i >= 0; i--)
	{
		m_particles[i]->onPrepare(dt);
		if (!m_particles[i]->isAlive())
		{
			delete m_particles[i];
			m_particles.erase(m_particles.begin() + i);
		}
	}

	generateParticles();
	
	// Update VBOs
	m_vertices.clear();
	m_lerpValues.clear();

	for (unsigned int i = 0; i < m_particles.size(); i++)
	{
		m_vertices.push_back(m_particles[i]->getPosition().x);
		m_vertices.push_back(m_particles[i]->getPosition().y);
		m_vertices.push_back(m_particles[i]->getPosition().z);

		//m_lerpValues.push_back(m_particles[i]->getElapsedPerc());
		m_lerpValues.push_back(1.0f);
		/*std::stringstream str;
		str << m_particles[i]->getElapsedPerc() << std::endl;
		OutputDebugString(str.str().c_str());*/
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	if (m_firstRun)
		glBufferData(GL_ARRAY_BUFFER, 3 * m_particles.size() * sizeof(GLfloat), &m_vertices[0], GL_STATIC_DRAW);
	else
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * m_particles.size() * sizeof(GLfloat), &m_vertices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_lerpValueBuffer);
	if (m_firstRun)
		glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(GLfloat), &m_lerpValues[0], GL_STATIC_DRAW);
	else
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_particles.size() * sizeof(GLfloat), &m_lerpValues[0]);

	if (m_firstRun)
		m_firstRun = false;
}

void Emitter::onRender()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	//glPointSize(10.0f);
	glPointParameterf(GL_POINT_SIZE_MIN, 2.0f);
	glPointParameterf(GL_POINT_SIZE_MAX, 20.0f);
	GLfloat pointDistAtt[] = {0.001f, 0.05f, 0.0f};
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, pointDistAtt);	

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	/*std::vector<GLdouble> texCoords;	
	for (unsigned int i = 0; i < m_particles.size(); i++)
	{
		texCoords.push_back(0.0);
		texCoords.push_back(0.0);
	}

	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * m_particles.size() * sizeof(GLdouble), &texCoords[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);*/

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferA);
	glVertexAttribPointer(1, 4, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferB);
	glVertexAttribPointer(2, 4, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_lerpValueBuffer);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);

	ShaderProgram *shaderProgram = m_engine->m_particleProgram;
	shaderProgram->bind();

	shaderProgram->sendMatrices();
	shaderProgram->sendUniform("texture0", 0);

	glDrawArrays(GL_POINTS, 0, m_particles.size());

	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glDisable(GL_POINT_SPRITE);

	shaderProgram = m_engine->m_basicProgram;
	shaderProgram->bind();
	shaderProgram->sendMatrices();
	//Draw emitter (debug)
	glBegin(GL_POINTS);
		glColor3d(0.0, 1.0, 0.0);
		glVertex3f(m_position->x, m_position->y, m_position->z);
	glEnd();
}

Emitter::~Emitter(void)
{
	for (unsigned int i = 0; i < m_particles.size(); i++)
		delete m_particles[i];

	m_particles.clear();
}
