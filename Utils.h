#pragma once
#include "glee.h"

class Utils
{
public:
	static void loadTexture(const char* filename, GLuint &texture);
	static GLfloat degreesToRadians(GLfloat degrees);
	static GLfloat radiansToDegrees(GLfloat radians);
};