#include "Enemy.h"

Enemy::Enemy(GameEngine *engine) : Object(engine)
{
	m_actor.load("models/players/varge/");

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.4f, 0.4f, 0.4f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.9f, 0.9f, 0.9f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(10.0f);
	m_actor.setMaterialProperties(props);
}

void Enemy::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);
	m_actor.onPrepare(dt);
}

void Enemy::onRender(ShaderProgram* shaderProgram)
{
	Object::onRender(shaderProgram);
	m_actor.onRender(shaderProgram);
}


Enemy::~Enemy(void)
{	
}
