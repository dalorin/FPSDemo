#include "Enemy.h"
#include "GameEngine.h"

Enemy::Enemy(GameEngine *engine, const char* modelPath) : Object(engine)
{
	m_actor.load(modelPath);

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.4f, 0.4f, 0.4f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.9f, 0.9f, 0.9f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(10.0f);
	m_actor.setMaterialProperties(props);
}

const char* Enemy::getType()
{
	return "Enemy";
}

void Enemy::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);
	m_actor.onPrepare(dt);
}

void Enemy::onRender()
{	
	Object::onRender();
	m_actor.onRender(m_engine->m_modelProgram);

	// DEBUG CODE - Draw collider
	SimpleBox* col = getCollider();
	std::vector<Vector3*> vec = col->getVertices(*m_position);
	m_engine->m_basicProgram->bind();
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < vec.size(); i++)
		glVertex3f(vec[i]->x, vec[i]->y, vec[i]->z);
	glEnd();
	
}

SimpleBox* Enemy::getCollider()
{
	return m_actor.getCollider();
}


Enemy::~Enemy(void)
{	
}
