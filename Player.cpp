#include "Player.h"
#include "GameEngine.h"

Player::Player(GameEngine* engine) : Object(engine)
{
}

void Player::shoot()
{	
	Camera *cam = m_engine->getCamera();
	Vector3 subject = cam->getSubject();
	Vector3 subjectRel = cam->getSubjectRelative();
	Vector3 position(subject.x + 1.0f, subject.y - 1.0f, subject.z - 4.0f);
	Vector3 velocity(subjectRel.x * 25.0f, subjectRel.y * 25.0f, subjectRel.z * 25.0f);
	Vector3 acceleration;
	m_engine->spawnEntity(BULLET, position, velocity, acceleration);
}

Player::~Player(void)
{
}
