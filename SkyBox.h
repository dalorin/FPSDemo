#pragma once
#include "objects.h"

class SkyBox :
	public Object
{
public:
	SkyBox(GameEngine *engine, 
		   const char* north, 
		   const char* south, 
		   const char* east, 
		   const char* west, 
		   const char* up, 
		   const char* down);

	void onPrepare(GLfloat dt);
	void onRender();

	~SkyBox(void);

private:
	GLuint *m_textures;
	GLuint m_vertexBuffer;
	GLuint m_texCoordBuffer;
};
