#pragma once
#include "objects.h"
#include "Model.h"

class GameEngine; // Forward declaration of GameEngine class.
class ShaderProgram; // Forward declaration of ShaderProgram class;

class Fighter : public Object
{
public:
	Fighter(GameEngine* engine);

	void onPrepare(GLfloat dt);
	void onRender(ShaderProgram* shaderProgram);

	~Fighter(void);

private:
	Model* m_model;
};
