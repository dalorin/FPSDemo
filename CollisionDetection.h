#pragma once
#include "objects.h"

struct SATResult {
    bool intersect;
    bool willIntersect;
    Vector3 minimumTranslationVector;
};

class CollisionDetection
{
public:	
	static SATResult detectCollision(Object& obj1, Object& obj2);
	static void projectObjectOntoAxis(Object& obj, SimpleBox& collider, Vector3& axis, GLfloat& min, GLfloat& max);
	static GLfloat intervalDistance(GLfloat min1, GLfloat max1, GLfloat min2, GLfloat max2);

private:
	static Vector3** m_edges;
};
