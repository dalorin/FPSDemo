#pragma once
#include "objects.h"
#include "MD3Actor.h"

class Enemy :
	public Object
{
public:
	Enemy(GameEngine *engine);

	void onPrepare(GLfloat dt);
	void onRender(ShaderProgram* shaderProgram);

	~Enemy(void);

private:
	MD3Actor m_actor;
};
