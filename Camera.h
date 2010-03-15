#pragma once
#include "objects.h"

class GameEngine; // Forward declaration of GameEngine class.

class Camera :
	public Object
{
public:
	Camera(GameEngine* engine);
	~Camera(void);

	void setSubject(GLfloat x, GLfloat y, GLfloat z);
	void adjustPitch(GLfloat angle);
	void adjustYaw(GLfloat angle);
	void resetPitch();
	void resetYaw();
	void moveForward();
	void moveBackward();
	void strafeLeft(GLfloat step);
	void strafeRight(GLfloat step);
	void reposition();
	Vector3 getSubject();
	Vector3 getSubjectRelative();

private:
	Vector3* m_subject;	
};
