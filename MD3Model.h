#pragma once
#include "glee.h"
#include <vector>
#include <map>
#include "objects.h"
#include <string>

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

class MD3Model
{
public:	
	MD3Model(const char* modelfilename, const char* textureFilename);

	// Load M3 model into memory
	bool load(const char* modelFilename, const char* textureFilename);
	void parseSkins(const char* modelPath);

	void setMaterialProperties(MaterialProps props);
	void onPrepare(float dt);
	void onRender(ShaderProgram *shaderProgram);
	void renderNormals(ShaderProgram *shaderProgram);

	~MD3Model(void);

private:
	std::vector<const char*> m_components;
	GLuint m_currentFrame;

	MD3Header m_header;

	std::map<const char*, std::vector<Frame>> m_frames;
	std::map<const char*, std::vector<Tag>> m_tags;
	std::map<const char*, std::vector<Surface>> m_surfaces;

	std::map<const char*, std::vector<GLuint>> m_vertexBuffers;
	std::map<const char*, std::vector<GLuint>> m_texCoordBuffers;
	std::map<const char*, std::vector<GLuint>> m_normalBuffers;
	std::map<std::string, GLuint> m_textures;
	MaterialProps m_materialProps;
};
