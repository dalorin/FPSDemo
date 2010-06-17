#pragma once
#include "objects.h"
#include "MD3Actor.h"

class Enemy :
	public Object
{
public:
	Enemy(GameEngine *engine, const char* modelPath);

	const char* getType();

	void onPrepare(GLfloat dt);
	void onRender();

	MD3Actor getActor() {return m_actor;}

	SimpleBox* getCollider();

	~Enemy(void);

private:
	MD3Actor m_actor;
};
