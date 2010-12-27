#pragma once
#include "glee.h"
#include <vector>
#include <map>
#include "objects.h"
#include <string>

namespace MD3 {

struct MD3Header {
	char ident[4];
	int version;
	char name[64];
	int flags;
	int numFrames;
	int numTags;
	int numSurfaces;
	int numSkins;
	int framesOffset;
	int tagsOffset;
	int surfacesOffset;
	int eofOffset;
};

struct MD3Shader {
	char name[64];
	GLuint shaderIndex;
};

struct Triangle {
	int indexes[3];
};

struct TexCoord {
	GLfloat s;
	GLfloat t;
};

struct MD3Vertex {
	short coord[3];
	char normal[2];
};

struct Vector {
	GLfloat v[3];
};

struct Frame {
	Vector minBounds;
	Vector maxBounds;
	Vector localOrigin;
	GLfloat radius;
	char name[16];
};

struct Tag {
	char name[64];
	Vector origin;
	Vector axis[3];
};

struct SurfaceHeader {
	char ident[4];
	char name[64];
	int flags;
	int numFrames;
	int numShaders;
	int numVerts;
	int numTris;
	int trisOffset;
	int shadersOffset;
	int texCoordsOffset;
	int vertsOffset;
	int endOffset;
};

struct Surface {
	SurfaceHeader header;
	std::vector<MD3Shader> shaders;
	std::vector<Triangle> triangles;
	std::vector<TexCoord> texCoords;
	std::vector<MD3Vertex> md3vertices;
	std::vector<Vector> vertices;
	std::vector<Vector> normals;
};

} // end namespace MD3

class MD3Model
{
public:	
	MD3Model();

	// Load M3 model into memory
	bool load(std::string modelFilename);
	void parseSkinFile(std::string modelFilename);
	void addLink(std::string tagName, MD3Model &model);

	void setMaterialProperties(MaterialProps props);
	void setAnimationParams(GLuint startFrame, GLuint endFrame, GLuint fps);
	bool hasAnimationFinished();

	Box* getCollider();

	void drawModel(ShaderProgram *shaderProgram);
	void onPrepare(float dt);
	void onRender(ShaderProgram *shaderProgram);
	void renderNormals(ShaderProgram *shaderProgram);

	~MD3Model(void);

private:
	std::vector<const char*> m_components;	
	//MD3::AnimationPhase m_animPhase;
	GLuint m_startFrame;
	GLuint m_endFrame;
	GLuint m_currentFrame;
	GLuint m_nextFrame;
	GLuint m_fps;
	float m_timeCount;
	float m_lerpValue;
	bool m_animationFinished;

	Box* m_collider;

	MD3::MD3Header m_header;

	std::vector<MD3::Frame> m_frames;
	std::vector<MD3::Tag> m_tags;
	std::vector<MD3::Surface> m_surfaces;
	std::map<std::string, MD3Model*> m_links;

	std::vector<std::vector<GLuint>> m_vertexBuffers;
	std::vector<GLuint> m_texCoordBuffers;
	std::vector<std::vector<GLuint>> m_normalBuffers;
	std::map<std::string, GLuint> m_textures;
	MaterialProps m_materialProps;
};
