#include "CollisionDetection.h"

Vector3** init_edge_array()
{
	Vector3** edges = new Vector3*[3];
	edges[0] = new Vector3(1, 0, 0);
	edges[1] = new Vector3(0, 1, 0);
	edges[2] = new Vector3(0, 0, 1);

	return edges;
}

Vector3** CollisionDetection::m_edges = init_edge_array();

SATResult CollisionDetection::detectCollision(Object& obj1, Object& obj2)
{	
	struct SATResult result;
	// Use bounding spheres to rapidly exclude possibility of collision.
    /*if ([Utils DistanceBetweenPoint:object1.position andPoint:object2.position] 
        >= object1.boundingCircleRadius + object2.boundingCircleRadius)
    {    
        result.intersect = NO;
    }
    GLfloat velocityMag = [object1.velocity getMagnitude] + [object2.velocity getMagnitude];
    if ([Utils DistanceBetweenPoint:object1.position andPoint:object2.position] 
        >= object1.boundingCircleRadius + object2.boundingCircleRadius + velocityMag)
    {    
        result.willIntersect = NO;
        return result;
    }*/
    result.intersect = true;
    result.willIntersect = true;
	//GLfloat minInterval = INFINITY;
	GLfloat minInterval = 100000.0f;
    Vector3 translationAxis;
    //printf("Running collision detection\n");
    // Find possible separation axes
    //NSMutableArray *edges = [NSMutableArray arrayWithArray:object1.edges];
    //[edges addObjectsFromArray:object2.edges];
    for (int i = 0; i < 3; i++)
    {

        //Vector3 axis = m_edges[i]->getNormal();
		Vector3 axis = *m_edges[i];
        axis.normalize();
        //printf("Normalized axis: %f,%f\n",axis.x,axis.y);
        
        GLfloat min1, max1, min2, max2;
        min1 = min2 = 10000.0f; max1 = max2 = -10000.0f;
		CollisionDetection::projectObjectOntoAxis(obj1, *(obj1.getCollider()), axis, min1, max1);
		CollisionDetection::projectObjectOntoAxis(obj2, *(obj2.getCollider()), axis, min2, max2);
        
		GLfloat interval = CollisionDetection::intervalDistance(min1, max1, min2, max2);
        //printf("Object1: %f-%f, Object2: %f-%f\n\n",min1,max1,min2,max2);
        
        if (interval >= 0)
        {
            result.intersect = false;
        }
        
		GLfloat velocityProjection = axis.dot(obj1.getVelocity() - obj2.getVelocity());
        
        if (velocityProjection < 0)
            min1 += velocityProjection;
        else
            max1 += velocityProjection;
        
		interval = CollisionDetection::intervalDistance(min1, max1, min2, max2);
        
        if (interval >= 0)
        {
            result.willIntersect = false;
        }
        
        //If the objects aren't intersecting and won't intersect, exit the loop
        if (!result.intersect && !result.willIntersect)
            break;
        
        //Calculate the minimum translation vector that we will use to push the objects apart
        interval = fabsf(interval);
        if (interval < minInterval)
        {            
            minInterval = interval;
            translationAxis = axis;
        
            //Calculate translation axis direction
            Vector3 d = obj1.getPosition() - obj2.getPosition();
            if (translationAxis.dot(d) < 0)
                translationAxis = -translationAxis;
        }
    }
    
    result.minimumTranslationVector = translationAxis * minInterval;

    return result;	
}

/**
 * projectObjectOntoAxis
 * Projects the given object onto the given axis. Returns object's minimum and maximum points on the axis.
 * Used in Seperating Axis Theorem ColDet.
 **/
void CollisionDetection::projectObjectOntoAxis(Object& obj, SimpleBox& collider, Vector3& axis, GLfloat& min, GLfloat& max)
{
	GLfloat dp;
	std::vector<Vector3*> &vertices = collider.getVertices(obj.getPosition());

	for (std::vector<Vector3*>::iterator vertex_it = vertices.begin();
		 vertex_it != vertices.end();
		 ++vertex_it)
    {
        dp = axis.dot(**vertex_it);
        if (dp < min)
            min = dp;
        if (dp > max)
            max = dp;

		delete *vertex_it;
    }
}

GLfloat CollisionDetection::intervalDistance(GLfloat min1, GLfloat max1, GLfloat min2, GLfloat max2)
{
    if (min1 < min2)
        return min2 - max1;
    else
        return min1 - max2;
}