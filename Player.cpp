#include "Player.h"
#include "GameEngine.h"

Player::Player(GameEngine* engine) : Object(engine)
{
}

void Player::shoot()
{
	Box* box = new Box(m_engine, 2.0f);	
	m_engine->spawnEntity(BULLET);
}

Player::~Player(void)
{
}
