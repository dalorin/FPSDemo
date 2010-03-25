#include "MD2Model.h"
#include <fstream>
#include <sstream>
#include "targa.h"
#include "Shader.h"
#include "Utils.h"
#include <map>

#define NUMVERTEXNORMALS 162

GLfloat vertexNormals[NUMVERTEXNORMALS][3] = {
	#include "anorms.h"
};

using namespace std;
using namespace MD2;

MD2Model::MD2Model(void)
{
}

MD2Model::MD2Model(const char* modelFilename, const char* textureFilename)
{
	load(modelFilename, textureFilename);
}

bool MD2Model::load(const char* modelFilename, const char* textureFilename)
{
	// Open the file
	ifstream modelFile(modelFilename, ios::binary);
	if (!modelFile)
	{
		stringstream str;
		str << "Invalid filename: " << modelFilename;
		MessageBox(NULL, str.str().c_str(), "Model error", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	// Read the model header into m_header
	modelFile.read(reinterpret_cast<char*>(&m_header), sizeof(MD2Header));

	// Resize data structures according to figures supplied in header
	m_skins.resize(m_header.numSkins);
	m_md2Texcoords.resize(m_header.numTextureCoords);
	m_texcoords.resize(m_header.numTextureCoords);
	m_triangles.resize(m_header.numTriangles);
	m_keyframes.resize(m_header.numFrames);

	for (vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		it->vertices.resize(m_header.numVertices);
		it->normals.resize(m_header.numVertices);
		it->md2Vertices.resize(m_header.numVertices);
	}

	// Read skin data into memory
	modelFile.seekg(m_header.skinOffset);
	modelFile.read(reinterpret_cast<char*>(&m_skins[0]), sizeof(Skin) * m_header.numSkins);

	// Read texture coordinate data into memory
	modelFile.seekg(m_header.texCoordOffset);
	modelFile.read(reinterpret_cast<char*>(&m_md2Texcoords[0]), sizeof(MD2TexCoord) * m_header.numTextureCoords);

	// Read triangles into memory
	modelFile.seekg(m_header.triangleOffset);
	modelFile.read(reinterpret_cast<char*>(&m_triangles[0]), sizeof(Triangle) * m_header.numTriangles);

	// Read keyframes into memory
	modelFile.seekg(m_header.frameOffset);

	for (vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		modelFile.read(reinterpret_cast<char*>(it->scale), sizeof(float) * 3);
		modelFile.read(reinterpret_cast<char*>(it->translate), sizeof(float) * 3);
		modelFile.read(reinterpret_cast<char*>(it->name), sizeof(char) * 16);
		modelFile.read(reinterpret_cast<char*>(&it->md2Vertices[0]), sizeof(MD2Vertex) * m_header.numVertices);
	}

	// Convert MD2 texture coords to OpenGL coordinate system
	for (unsigned int i = 0; i < m_md2Texcoords.size(); ++i)
	{
		m_texcoords[i].s = (GLfloat)m_md2Texcoords[i].s / (GLfloat)m_header.skinWidth;
		m_texcoords[i].t = 1.0f - ((GLfloat)m_md2Texcoords[i].t / (GLfloat)m_header.skinHeight);
	}

	// Decompress keyframe vertices and swap Y and Z axes
	for (vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		for (int i = 0; i < m_header.numVertices; ++i)
		{
			it->vertices[i].v[0] = (it->scale[0] * it->md2Vertices[i].v[0]) + it->translate[0];
			it->vertices[i].v[2] = (it->scale[1] * it->md2Vertices[i].v[1]) + it->translate[1];
			it->vertices[i].v[1] = (it->scale[2] * it->md2Vertices[i].v[2]) + it->translate[2];
			it->normals[i].v[0] = vertexNormals[it->md2Vertices[i].lightNormalIndex][0];
			it->normals[i].v[2] = vertexNormals[it->md2Vertices[i].lightNormalIndex][1];
			it->normals[i].v[1] = vertexNormals[it->md2Vertices[i].lightNormalIndex][2];
		}
	}

	//calculateNormals();

	reorganiseVertices();
	
	Utils::loadTexture(textureFilename, m_texture);

	m_interpolatedKeyFrame = m_keyframes[0];

	// Load vertex data into buffer.
	glGenBuffers(1, &m_vertexBuffer);	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);	
	glBufferData(GL_ARRAY_BUFFER, m_interpolatedKeyFrame.vertices.size() * sizeof(Vertex), &m_interpolatedKeyFrame.vertices[0], GL_DYNAMIC_DRAW);

	// Load texcoord data into buffer.
	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof(TexCoord), &m_texcoords[0], GL_STATIC_DRAW);

	// Load vertex normal data into buffer.
	glGenBuffers(1, &m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_interpolatedKeyFrame.normals.size() * sizeof(Vertex), &m_interpolatedKeyFrame.normals[0], GL_DYNAMIC_DRAW);

	return true;
}

// Iterate through keyframes rebuilding vertices from triangle index info
void MD2Model::reorganiseVertices()
{
	vector<Vertex> tempVertices;
	vector<TexCoord> tempTexCoords;
	vector<Vertex> tempNormals;

	bool texCoordsDone = false;

	for (vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		for (unsigned int i = 0; i < m_triangles.size(); ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				tempVertices.push_back(it->vertices[m_triangles[i].vertexIndex[j]]);
				tempNormals.push_back(it->normals[m_triangles[i].vertexIndex[j]]);				
				
				if (!texCoordsDone)
					tempTexCoords.push_back(m_texcoords[m_triangles[i].texCoordIndex[j]]);
			}
		}
		texCoordsDone = true;
		
		it->vertices = tempVertices;
		it->normals = tempNormals;
	}

	m_texcoords = tempTexCoords;
}

/*void MD2Model::calculateNormals()
{
	for (std::vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		KeyFrame* keyframe = &(*it);
		for (unsigned int i = 0; i < keyframe->vertices.size(); i += 3)
		{
			Vertex* v1 = &keyframe->vertices[i];
			Vertex* v2 = &keyframe->vertices[i+1];
			Vertex* v3 = &keyframe->vertices[i+2];

			Vector3 faceNormal;
			Vector3 a(v2->v[0] - v1->v[0], v2->v[1] - v1->v[1], v2->v[2] - v1->v[2]);
			Vector3 b(v3->v[0] - v1->v[0], v3->v[1] - v1->v[1], v3->v[2] - v1->v[2]);

			faceNormal = a.cross(b);
			faceNormal.normalize();

			Vertex normal;
			normal.v[0] = faceNormal.x;
			normal.v[1] = faceNormal.y;
			normal.v[2] = faceNormal.z;

			for (int i = 0; i < 3; i++)
				keyframe->normals.push_back(normal);
		}
	}	
}*/

void MD2Model::calculateNormals()
{
	std::map<short, int> vertexIncidences; //Stores number of triangles in which a vertex appears.
	std::map<short, std::vector<Triangle*>> vertexTriangles; //Stores references to triangles in which a vertex appears.
	std::map<Triangle*, Vector3> surfaceNormals; //Stores surface normals of triangles.

	//For each keyframe
	for (std::vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		KeyFrame* keyframe = &(*it);
		//For each triangle
		for (unsigned int i = 0; i < m_triangles.size(); i++)
		{
			Triangle* triangle = &m_triangles[i];			
			//For each triangle vertex
			for (int j = 0; j < 3; j++)
			{
				//  Increment incidence count for each vertex index
				short vertexIndex = triangle->vertexIndex[j];
				if (vertexIncidences.find(vertexIndex) != vertexIncidences.end())
					vertexIncidences[vertexIndex]++;					
				else
					vertexIncidences[vertexIndex] = 1;
				
				//  Store reference to triangle in each vertex				
				vertexTriangles[vertexIndex].push_back(triangle);
			}
			//  Calculate and store surface normal
			Vertex* v1 = &keyframe->vertices[triangle->vertexIndex[0]];
			Vertex* v2 = &keyframe->vertices[triangle->vertexIndex[1]];
			Vertex* v3 = &keyframe->vertices[triangle->vertexIndex[2]];

			Vector3 faceNormal;
			Vector3 a(v2->v[0] - v1->v[0], v2->v[1] - v1->v[1], v2->v[2] - v1->v[2]);
			Vector3 b(v3->v[0] - v1->v[0], v3->v[1] - v1->v[1], v3->v[2] - v1->v[2]);

			faceNormal = a.cross(b);
			faceNormal.normalize();
			surfaceNormals[triangle] = faceNormal;
		}
	}

	//For each keyframe
	for (std::vector<KeyFrame>::iterator it = m_keyframes.begin();
		 it != m_keyframes.end();
		 ++it)
	{
		KeyFrame* keyframe = &(*it);
		// For each vertex
		for (unsigned short i = 0; i < keyframe->md2Vertices.size(); i++)
		{
			Vector3 sum(0.0f, 0.0f, 0.0f);
			std::vector<Triangle*> triangles = vertexTriangles[i];
			// For each triangle to which this vertex belongs
			for (std::vector<Triangle*>::iterator triangle_it = triangles.begin();
				 triangle_it != triangles.end();
				 ++triangle_it)
			{
				Triangle* triangle = *triangle_it;
				// Sum surface normals
				sum = sum + surfaceNormals[triangle];				
			}
			// Divide by incidence count to get vertex normal and store it
			sum = sum / (GLfloat)vertexIncidences[i];
			sum.normalize();

			Vertex normal;
			normal.v[0] = sum.x;
			normal.v[1] = sum.y;
			normal.v[2] = sum.z;

			keyframe->normals.push_back(normal);
		}
	}	
}

void MD2Model::setMaterialProperties(MaterialProps props)
{
	m_materialProps = props;
}

void MD2Model::onPrepare(float dt)
{
	m_interpolatedKeyFrame = m_keyframes[0];
}

void MD2Model::onRender(ShaderProgram *shaderProgram)
{	
	glPushMatrix();
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);
		
	glBindTexture(GL_TEXTURE_2D, m_texture);

	shaderProgram->sendMatrices();
	shaderProgram->sendUniform("texture0", 0);
	shaderProgram->sendUniform("lerp_value", 0.0f);
	shaderProgram->sendMaterialProps(m_materialProps);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	glDrawArrays(GL_TRIANGLES, 0, m_interpolatedKeyFrame.vertices.size());
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(0);

	glPopMatrix();

	//renderNormals(shaderProgram);
}

void MD2Model::renderNormals(ShaderProgram *shaderProgram)
{
	glPushMatrix();

	glBegin(GL_LINES);
	for (unsigned int i = 0; i < m_interpolatedKeyFrame.vertices.size(); i++)
	{
		glVertex3f(
			m_interpolatedKeyFrame.vertices[i].v[0],
			m_interpolatedKeyFrame.vertices[i].v[1],
			m_interpolatedKeyFrame.vertices[i].v[2]
		);
		glVertex3f(
			m_interpolatedKeyFrame.vertices[i].v[0] + m_interpolatedKeyFrame.normals[i].v[0],
			m_interpolatedKeyFrame.vertices[i].v[1] + m_interpolatedKeyFrame.normals[i].v[1],
			m_interpolatedKeyFrame.vertices[i].v[2] + m_interpolatedKeyFrame.normals[i].v[2]
		);
	}
	glEnd();

	glPopMatrix();
}

MD2Model::~MD2Model(void)
{
}
