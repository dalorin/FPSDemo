#pragma once
#include "Actor.h"
#include "MD3Actor.h"

class Enemy :
	public Actor
{
public:
	Enemy(GameEngine *engine, const char* modelPath);

	const char* getType();

	void onPrepare(GLfloat dt);
	void onRender();

	MD3Actor* getActor() {return m_actor;}

	void hit();
	void kill();
	void attack();

	Box* getCollider();

	~Enemy(void);

protected:
	MD3Actor* m_actor;
};
