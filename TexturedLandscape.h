#pragma once
#include "objects.h"

class TexturedLandscape : public Object
{
public:	
	TexturedLandscape(GameEngine* engine);

	void onRender();

	~TexturedLandscape(void);

private:
	GLuint m_buffer; //vertex buffer
	GLuint m_texCoordBuffer; //texture coordinate buffer
	GLuint m_normalBuffer; //vertex normal buffer
	GLuint m_texture; //texture buffer
};
