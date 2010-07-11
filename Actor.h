#pragma once
#include "objects.h"

#define ACTOR_IDLE			1
#define ACTOR_WALKING		2
#define ACTOR_RUNNING		4
#define ACTOR_ATTACKING		8
#define ACTOR_DEAD			16

class Actor :
	public Object
{
public:
	Actor(GameEngine* engine);
	~Actor(void);

	virtual void hit();

protected:
	int m_health;
	unsigned char m_state;
};
