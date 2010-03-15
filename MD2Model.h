#pragma once
#include "glee.h"
#include <vector>
#include "objects.h"

struct MD2Header {
	char ident[4];
	int version;
	int skinWidth;
	int skinHeight;
	int frameSize;
	int numSkins;
	int numVertices;
	int numTextureCoords;
	int numTriangles;
	int numGLCmds;
	int numFrames;
	int skinOffset;
	int texCoordOffset;
	int triangleOffset;
	int frameOffset;
	int GLCmdOffset;
	int eofOffset;
};

struct Skin {
	char filename[64];
};

struct MD2TexCoord {
	short s;
	short t;
};

struct TexCoord {
	GLfloat s;
	GLfloat t;
};

struct Triangle {
	short vertexIndex[3];
	short texCoordIndex[3];
};

struct MD2Vertex {
	unsigned char v[3];
	unsigned char lightNormalIndex;
};

struct Vertex {
	GLfloat v[3];
};

struct KeyFrame {
	float scale[3];
	float translate[3];
	char name[16];
	std::vector<Vertex> vertices;
	std::vector<MD2Vertex> md2Vertices;
	std::vector<Vertex> normals;	
};

class MD2Model
{
public:
	// Constructors
	MD2Model(void);
	MD2Model(const char* modelfilename, const char* textureFilename);

	// Load M2 model into memory
	bool load(const char* modelFilename, const char* textureFilename);
	void reorganiseVertices();
	void calculateNormals();
	void setMaterialProperties(MaterialProps props);
	void onPrepare(float dt);
	void onRender(ShaderProgram *shaderProgram);
	void renderNormals(ShaderProgram *shaderProgram);
	~MD2Model(void);

private:
	MD2Header m_header;
	std::vector<Skin> m_skins;
	std::vector<TexCoord> m_texcoords;
	std::vector<MD2TexCoord> m_md2Texcoords;
	std::vector<Triangle> m_triangles;
	std::vector<KeyFrame> m_keyframes;
	KeyFrame m_interpolatedKeyFrame;
	GLuint m_vertexBuffer;
	GLuint m_texCoordBuffer;
	GLuint m_normalBuffer;
	GLuint m_texture;
	MaterialProps m_materialProps;
};

