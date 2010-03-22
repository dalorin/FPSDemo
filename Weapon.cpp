#include "Weapon.h"

Weapon::Weapon(GameEngine* engine) : Object(engine)
{
	m_model = new MD2Model("hk_53_fixed.md2", "hk53_n.tga");

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.5f, 0.5f, 0.5f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(20.0f);
	m_model->setMaterialProperties(props);	
}

void Weapon::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);
	m_model->onPrepare(dt);
}

void Weapon::onRender(ShaderProgram* shaderProgram)
{
	Object::onRender(shaderProgram);
	m_model->onRender(shaderProgram);
}

Weapon::~Weapon(void)
{
	delete m_model;
}