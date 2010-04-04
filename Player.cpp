#include "Player.h"
#include "GameEngine.h"

Player::Player(GameEngine* engine) : Object(engine)
{
}

void Player::shoot()
{	
	m_engine->spawnEntity(BULLET);
}

Player::~Player(void)
{
}
