#pragma once
#include "objects.h"
#include "MD2Model.h"

class Weapon :
	public Object
{
public:
	Weapon(GameEngine *engine);

	void attack();

	void onPrepare(GLfloat dt);
	void onRender();

	virtual void drawBoundingSphere();

	~Weapon(void);
private:
	MD2Model* m_model;
};
