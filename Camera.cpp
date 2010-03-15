#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>

GLfloat degreesToRadians(GLfloat degrees)
{
	return degrees * (GLfloat)M_PI / 180.0f;
}

Camera::Camera(GameEngine* engine) : Object(engine)
{
	m_subject = new Vector3(0.0f, 10.0f, 3.0f);
	setPosition(0.0f, 10.0f, 4.0f);
	m_pitch = 0.0f;
	m_yaw = 0.0f;
}

void Camera::setSubject(GLfloat x, GLfloat y, GLfloat z)
{
	m_subject->x = x;
	m_subject->y = y;
	m_subject->z = z;
}

void Camera::adjustPitch(GLfloat angle)
{
	m_pitch += angle;

	if (m_pitch > 90.0f)
		m_pitch = 90.0f;
	else if (m_pitch < -90.0f)
		m_pitch = -90.0f;
}

void Camera::adjustYaw(GLfloat angle)
{
	m_yaw += angle;
	
	if (m_yaw >= 360.0f)
		m_yaw -= 360.0f;
	else if (m_yaw < 0.0f)
		m_yaw += 360.0f;
}

void Camera::resetYaw()
{
	m_yaw = 0.0f;
}

void Camera::resetPitch()
{
	m_pitch = 0.0f;
}

void Camera::moveForward()
{
	Vector3 newPos = getPosition() + getSubjectRelative();
	setPosition(newPos.x, getPosition().y, newPos.z);
	reposition();
}

void Camera::moveBackward()
{
	Vector3 newPos = getPosition() - getSubjectRelative();
	setPosition(newPos.x, getPosition().y, newPos.z);
	reposition();
}

void Camera::strafeLeft(GLfloat step)
{
	Vector3 pos = getPosition();
	pos.x += cosf(degreesToRadians(-m_yaw));
	pos.z += sinf(degreesToRadians(-m_yaw));
	setPosition(pos);
	reposition();
}

void Camera::strafeRight(GLfloat step)
{
	Vector3 pos = getPosition();
	pos.x -= cosf(degreesToRadians(-m_yaw));
	pos.z -= sinf(degreesToRadians(-m_yaw));
	setPosition(pos);
	reposition();
}

Vector3 Camera::getSubject()
{
	return *m_subject;
}

Vector3 Camera::getSubjectRelative()
{
	return *m_subject - *m_position;
}

void Camera::reposition()
{
	GLfloat x = sinf(degreesToRadians(m_yaw));
	GLfloat y = sinf(degreesToRadians(m_pitch));
	GLfloat z = cosf(degreesToRadians(m_yaw));
	Vector3 subjectLocal(x, y, z);
	Vector3 subjectWorld = getPosition() + subjectLocal;

/*	std::stringstream str;
	str << "Local X: " << x << " Local Y: " << y << " Local Z: " << z << std::endl;
	str << "World X: " << subjectWorld.x << " World Y: " << subjectWorld.y << " World Z: " << subjectWorld.z << std::endl;
	OutputDebugString(str.str().c_str());*/
	setSubject(subjectWorld.x, subjectWorld.y, subjectWorld.z);
}

Camera::~Camera(void)
{
}
