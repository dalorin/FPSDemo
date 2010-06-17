#pragma once
#include "objects.h"

class GameEngine; // Forward declaration of GameEngine class.
class Weapon; // Forward declaration of Weapon class.

class Camera :
	public Object
{
public:
	Camera(GameEngine* engine);
	~Camera(void);

	const char* getType();

	void setSubject(GLfloat x, GLfloat y, GLfloat z);
	void adjustPitch(GLfloat angle);
	void adjustYaw(GLfloat angle);
	void resetPitch();
	void resetYaw();
	void moveForward();
	void moveBackward();
	void strafeLeft(GLfloat step);
	void strafeRight(GLfloat step);
	void adjustBob(bool forward);
	GLfloat getBob() { return m_bobValue; }
	void reposition();
	Vector3 getSubject();
	Vector3 getSubjectRelative();	
	
private:
	Vector3* m_subject;
	bool m_bobUp;
	GLfloat m_bobValue;
};
