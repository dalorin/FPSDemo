#include "Enemy.h"
#include "GameEngine.h"
#include "Utils.h"

Enemy::Enemy(GameEngine *engine, const char* modelPath) : Actor(engine)
{
	m_actor = new MD3Actor();
	m_actor->load(modelPath);

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.4f, 0.4f, 0.4f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.9f, 0.9f, 0.9f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(10.0f);
	m_actor->setMaterialProperties(props);
}

const char* Enemy::getType()
{
	return "Enemy";
}

/**
 * Cause the enemy to sustain damage.
 **/  
void Enemy::hit()
{
	m_health -= 10;
	if (m_health <= 0)
		kill();
}

/** 
 * Kill the enemy.
 **/  
void Enemy::kill()
{
	//Object::kill();
	setAlive(false);
	m_state = ACTOR_DEAD;
	if (getActor()->getLowerAnimation() != AnimationPhase::BOTH_DEATH1)
	{
		getActor()->setLowerAnimation(AnimationPhase::BOTH_DEATH1);
		getActor()->setUpperAnimation(AnimationPhase::BOTH_DEATH1);
		getActor()->setNextUpperAnimation(AnimationPhase::BOTH_DEAD1);
		getActor()->setNextLowerAnimation(AnimationPhase::BOTH_DEAD1);
	}
}

/** 
 * Launch an attack.
 * Defaults to "shoot" but could be modified to accept an attack type e.g. "melee", "secondary".
 **/  
void Enemy::attack()
{
	//m_engine->spawnEntity(BULLET);
}

void Enemy::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);

	Vector3 camPos = m_engine->getCameraPos();
	setYaw(Utils::radiansToDegrees(atanf((getPosition().x - camPos.x)/(getPosition().z - camPos.z))) - 90.0f);
	Vector3 relative = camPos - getPosition();
	relative.normalize();
	setVelocity(relative.x, 0, relative.z);
	m_actor->onPrepare(dt);
}

void Enemy::onRender()
{	
	Object::onRender();
	m_actor->onRender(m_engine->m_modelProgram);

	// DEBUG CODE - Draw collider
	/*Box* col = getCollider();
	std::vector<Vector3*> vec = col->getVertices(*m_position);
	m_engine->m_basicProgram->bind();
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < vec.size(); i++)
		glVertex3f(vec[i]->x, vec[i]->y, vec[i]->z);
	glEnd();*/
	
}

Box* Enemy::getCollider()
{
	return m_actor->getCollider();
}


Enemy::~Enemy(void)
{	
	delete m_actor;
}
