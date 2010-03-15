#pragma once
#include "objects.h"

class Landscape : public Object
{
public:
	Landscape(GameEngine* engine);

	void onRender(ShaderProgram *program);

	~Landscape(void);

private:
	GLuint m_buffer;
	GLuint m_color;
};
