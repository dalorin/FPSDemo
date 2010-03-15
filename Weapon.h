#pragma once
#include "objects.h"
#include "Model.h"

class Weapon :
	public Object
{
public:
	Weapon(GameEngine *engine);

	void attack();

	void onPrepare(GLfloat dt);
	void onRender(ShaderProgram* shaderProgram);	

	~Weapon(void);
private:
	Model* m_model;
};
