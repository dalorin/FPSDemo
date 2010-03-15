#pragma once

#include <windows.h>
#include "glee.h"
#include <gl/glu.h>
#include <vector>
#include <math.h>

class GameEngine; // Forward declaration of GameEngine class.
class ShaderProgram; // Forward declaration of ShaderProgram class.

#define BUFFER_OFFSET(i) ((char*)NULL + i)

struct Vector3 
{
	GLfloat x, y, z;

	Vector3()
	{
		x = y = z = 0.0f;
	}

	Vector3(GLfloat x, GLfloat y, GLfloat z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	GLfloat length()
	{
		return sqrtf(powf(this->x, 2.0f) + powf(this->y, 2.0f) + powf(this->z, 2.0f));
	}

	void normalize()
	{
		GLfloat len = length();

		x /= len;
		y /= len;
		z /= len;
	}

	Vector3 Vector3::operator+(Vector3 &otherVector)
	{
		Vector3 ret;
		ret.x = x + otherVector.x;
		ret.y = y + otherVector.y;
		ret.z = z + otherVector.z;

		return ret;
	}

	Vector3 Vector3::operator-(Vector3 &otherVector)
	{
		Vector3 ret;
		ret.x = x - otherVector.x;
		ret.y = y - otherVector.y;
		ret.z = z - otherVector.z;

		return ret;
	}

	Vector3 Vector3::operator*(const GLfloat &multiple)
	{
		Vector3 ret;
		ret.x = x * multiple;
		ret.y = y * multiple;
		ret.z = z * multiple;

		return ret;
	}

	Vector3 Vector3::operator/(const GLfloat &multiple)
	{
		Vector3 ret;
		ret.x = x / multiple;
		ret.y = y / multiple;
		ret.z = z / multiple;

		return ret;
	}

	Vector3 Vector3::cross(Vector3 &otherVector)
	{
		Vector3 ret;
		ret.x = (y * otherVector.z) - (z * otherVector.y);
		ret.y = (z * otherVector.x) - (x * otherVector.z);
		ret.z = (x * otherVector.y) - (y * otherVector.x);

		return ret;
	}
};

struct Color
{
	Color()
	{
		r = g = b = a = 0.0;
	}
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
};

class MaterialProps
{
public:	
	enum PropType {AMBIENT, DIFFUSE, SPECULAR, EMISSIVE};
	void setProperty(PropType type, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
	{
		Color* prop = getProp(type);
		
		prop->r = r;
		prop->g = g;
		prop->b = b;
		prop->a = a;
	}

	void setShininess(GLfloat shininess)
	{
		m_shininess = shininess;
	}

	Color getProperty(PropType type)
	{
		return *(getProp(type));
	}

	GLfloat getShininess()
	{
		return m_shininess;
	}

private:
	Color* getProp(PropType type)
	{
		Color* prop;
		switch(type)
		{
			case AMBIENT:
			{
				prop = &m_ambient;
				break;
			}
			case DIFFUSE:
			{
				prop = &m_diffuse;
				break;		
			}
			case SPECULAR:
			{
				prop = &m_specular;
				break;		
			}
			case EMISSIVE:
			{
				prop = &m_emissive;
				break;
			}
			default:
			{
				MessageBox(NULL, "Material Error", "Invalid material property type", MB_ICONERROR | MB_OK);
				exit(-1);
			}
		}

		return prop;
	}

	Color m_ambient;
	Color m_diffuse;
	Color m_specular;
	Color m_emissive;
	GLfloat m_shininess;
};

class Object
{
public:
	Object(GameEngine* engine); //constructor
	~Object(); //destructor
	virtual void onPrepare(GLfloat dt);
	virtual void onRender(ShaderProgram *shaderProgram);
	virtual void onPostRender();

	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	void setPosition(Vector3 position);
	Vector3 getPosition();

	void setScale(GLfloat x, GLfloat y, GLfloat z);
	Vector3 getScale();

	void setVelocity(GLfloat x, GLfloat y, GLfloat z);
	Vector3 getVelocity();

	void setAcceleration(GLfloat x, GLfloat y, GLfloat z);
	Vector3 getAcceleration();

	virtual void adjustPitch(GLfloat angle);
	virtual void adjustYaw(GLfloat angle);

	GLfloat getPitch() {return m_pitch;}
	GLfloat getYaw() {return m_yaw;}
	bool isAlive() {return m_alive;}

protected:
	Vector3* m_position;
	Vector3* m_scale;
	Vector3* m_velocity;
	Vector3* m_acceleration;

	GLfloat m_yaw;
	GLfloat m_pitch;

	MaterialProps m_materialProps;
	
	std::vector<GLfloat> m_vertices;
	std::vector<GLdouble> m_colors;

	GameEngine* m_engine; // Pointer to engine that spawned object.

	bool m_alive; // If false, engine will delete object. 
};

class Box : public Object
{
public:
	Box(GameEngine* engine, GLfloat length); //constructor
	void onPrepare(GLfloat dt);
	void onRender(ShaderProgram *shaderProgram);
private:
	GLfloat length;
	GLfloat m_rotation;
	GLuint m_vertexBuffer, m_colorBuffer;
};

class Crosshair : public Object
{
public:
	Crosshair(GameEngine* engine); //constructor
	void onRender(ShaderProgram *shaderProgram);	
private:
	GLuint m_buffer;
	GLuint m_texture;
	GLuint m_textureCoords;
};