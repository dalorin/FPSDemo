#pragma once
#include "objects.h"
#include "MD2Model.h"

class GameEngine; // Forward declaration of GameEngine class.
class ShaderProgram; // Forward declaration of ShaderProgram class;

class Fighter : public Object
{
public:
	Fighter(GameEngine* engine);

	const char* getType();

	void onPrepare(GLfloat dt);
	void onRender();

	~Fighter(void);

private:
	MD2Model* m_model;
};
