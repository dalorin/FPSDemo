#include "Fighter.h"

class ShaderProgram; // Forward declaration of ShaderProgram class.

Fighter::Fighter(GameEngine* engine) : Object(engine)
{
	m_model = new Model("models/players/varge/", "models/varge/visor.tga");

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.4f, 0.4f, 0.4f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.9f, 0.9f, 0.9f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(90.0f);
	m_model->setMaterialProperties(props);
}

void Fighter::onPrepare(GLfloat dt)
{
	Object::onPrepare(dt);
	m_model->onPrepare(dt);
}

void Fighter::onRender(ShaderProgram* shaderProgram)
{
	Object::onRender(shaderProgram);
	m_model->onRender(shaderProgram);
}

Fighter::~Fighter(void)
{
	delete m_model;
}
