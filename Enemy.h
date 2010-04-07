#pragma once
#include "objects.h"
#include "MD3Actor.h"

class Enemy :
	public Object
{
public:
	Enemy(GameEngine *engine, const char* modelPath);

	void onPrepare(GLfloat dt);
	void onRender();

	~Enemy(void);

private:
	MD3Actor m_actor;
};
