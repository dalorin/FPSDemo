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

	Vector3 Vector3::operator-()
	{
		Vector3 ret;
		ret.x = -x;
		ret.y = -y;
		ret.z = -z;

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

	GLfloat Vector3::dot(Vector3 &otherVector)
	{
		return (this->x * otherVector.x) + (this->y * otherVector.y) + (this->z * otherVector.z);
	}
};

struct Sphere
{
	Vector3 position;
	GLfloat radius;

	Sphere::Sphere(Vector3 pos, GLfloat rad)
	{
		position = pos;
		radius = rad;
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

class SimpleBox
{
public:
	SimpleBox(GLfloat length); //constructor
	SimpleBox(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2); //constructor for boxes with non-uniform edge length

	void initBox(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2);

	GLfloat getX1() {return m_x1;}
	GLfloat getY1() {return m_y1;}
	GLfloat getZ1() {return m_z1;}
	GLfloat getX2() {return m_x2;}
	GLfloat getY2() {return m_y2;}
	GLfloat getZ2() {return m_z2;}

	std::vector<Vector3*> getVertices(Vector3 translation);

protected:
	GLfloat m_x1, m_y1, m_z1, m_x2, m_y2, m_z2;
};

class Object
{
public:
	Object(GameEngine* engine); //constructor
	~Object(); //destructor

	virtual const char* getType();

	virtual void onPrepare(GLfloat dt);
	virtual void onRender();
	virtual void onPostRender();

	virtual void drawBoundingSphere();

	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	void setPosition(Vector3 position);
	Vector3 getPosition();

	void setScale(GLfloat x, GLfloat y, GLfloat z);
	Vector3 getScale();

	void setVelocity(GLfloat x, GLfloat y, GLfloat z);
	void setVelocity(Vector3 velocity);
	Vector3 getVelocity();

	void setAcceleration(GLfloat x, GLfloat y, GLfloat z);
	void setAcceleration(Vector3 acceleration);
	Vector3 getAcceleration();

	virtual void adjustPitch(GLfloat angle);
	virtual void adjustYaw(GLfloat angle);

	void setPitch(GLfloat angle);
	void setYaw(GLfloat angle);

	GLfloat getPitch() {return m_pitch;}
	GLfloat getYaw() {return m_yaw;}

	void setAlive(bool alive) {m_alive = alive;}
	bool isAlive() {return m_alive;}
	void setDelete(bool can_delete) {m_can_delete = can_delete;}
	bool canDelete() {return m_can_delete;}
	
	virtual bool collides() {return true;}

	virtual void kill();

	virtual SimpleBox* getCollider();

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

	bool m_alive; // If false, object is dead.
	bool m_can_delete; // If true, engine will delete object.

	GLfloat m_time_since_death;

	GLUquadricObj *m_quadratic;

	// Pointer to another object used when detecting collisions.
	// Usually a bounding box or sphere. Could be a pointer to the object itself.
	// At the moment this has to be a SimpleBox.
	SimpleBox* m_collider;
};

class Box : public Object, public SimpleBox
{
public:
	Box(GameEngine* engine, GLfloat length); //constructor
	Box(GameEngine* engine, GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2); //constructor for boxes with non-uniform edge length
	Box(GameEngine* engine, SimpleBox box); //constructor from SimpleBox
	
	void initBox();

	void onPrepare(GLfloat dt);
	void onRender();
private:
	GLuint m_vertexBuffer, m_colorBuffer;
};

class Crosshair : public Object
{
public:
	Crosshair(GameEngine* engine); //constructor
	void onRender();	
private:
	GLuint m_buffer;
	GLuint m_texture;
	GLuint m_textureCoords;
};