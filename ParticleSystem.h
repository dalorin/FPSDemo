#pragma once
#include "objects.h"
#include <vector>

class Particle :
	public Object
{
public:
	Particle(GameEngine *engine, GLuint texture, GLfloat lifetime);	

	const char* getType();

	bool isAlive();
	GLuint getTexture() { return m_texture; }
	GLdouble* getStartColor() { return m_startColor; }
	GLdouble* getEndColor() { return m_endColor; }
	GLfloat getElapsedPerc() { return m_elapsed / m_lifetime; }

	bool collides() {return false;}

	void onPrepare(GLfloat dt);

	~Particle();

private:
	GLuint m_texture;
	GLdouble m_startColor[4];
	GLdouble m_endColor[4];
	GLfloat m_lifetime;
	GLfloat m_elapsed;
};

class Emitter :
	public Object
{
public:
	Emitter(GameEngine *engine, 
			int maxParticles, 
			GLfloat lifetimeMean, 
			GLfloat lifetimeSpread, 
			GLfloat velocitySpread,
			const char* textureFilename);

	const char* getType();

	GLfloat getRandomValue(GLfloat mean, GLfloat spread);

	void generateParticles();

	void onPrepare(GLfloat dt);
	void onRender();

	~Emitter(void);

private:
	std::vector<Particle*> m_particles;
	std::vector<GLfloat> m_lerpValues;

	int m_maxParticles;
	GLfloat m_lifetimeMean;
	GLfloat m_lifetimeSpread;
	GLfloat m_velocitySpread;
	GLuint m_texture, m_vertexBuffer, m_colorBufferA, m_colorBufferB, m_lerpValueBuffer;
	bool m_firstRun;
};
