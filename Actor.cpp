#include "Actor.h"

Actor::Actor(GameEngine* engine) : Object(engine)
{
	m_state = ACTOR_IDLE;
	m_health = 100;
}

void Actor::hit()
{
	m_health -= 10;
	if (m_health <= 0)
		setAlive(false);
}

Actor::~Actor(void)
{
}
