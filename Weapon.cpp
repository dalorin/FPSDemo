#include "Weapon.h"
#include "GameEngine.h"

Weapon::Weapon(GameEngine* engine) : Object(engine)
{
	m_model = new MD2Model("hk_53.md2", "hk53_n.tga");

	//Set material properties.
	MaterialProps props;
	props.setProperty(MaterialProps::AMBIENT, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::DIFFUSE, 0.7f, 0.7f, 0.7f, 1.0f);
	props.setProperty(MaterialProps::SPECULAR, 0.5f, 0.5f, 0.5f, 1.0f);
	props.setProperty(MaterialProps::EMISSIVE, 0.0f, 0.0f, 0.0f, 1.0f);
	props.setShininess(20.0f);
	m_model->setMaterialProperties(props);	

	m_quadratic = gluNewQuadric();
	gluQuadricNormals(m_quadratic, GLU_SMOOTH);
	gluQuadricTexture(m_quadratic, GL_TRUE);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		MessageBox(NULL, "Error creating quadratic", "OpenGL Error!", MB_ICONERROR | MB_OK);
	}
}

void Weapon::onPrepare(GLfloat dt)
{
	// Don't need the standard onPrepare functionality. The weapon won't be subject to vel or acc and shouldn't die.
	//Object::onPrepare(dt);
	m_model->onPrepare(dt);
}

void Weapon::onRender()
{	
	Object::onRender();

	m_model->onRender(m_engine->m_modelProgram);
}

void Weapon::drawBoundingSphere()
{
	Object::onRender();

	
		glColor3d(1.0, 0.0, 0.0);	
		gluSphere(m_quadratic, m_model->getBoundingSphereRadius(), 32, 32);
	

	glPopMatrix();
}

Weapon::~Weapon(void)
{
	delete m_model;
}