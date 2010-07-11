#include "MD3Model.h"
#include <fstream>
#include <sstream>
#include "targa.h"
#include "Shader.h"
#include "Utils.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace MD3;

MD3Model::MD3Model()
{
	m_startFrame = 0;
	m_endFrame = 0;
	m_currentFrame = 0;
	m_nextFrame = 0;
	m_timeCount = 0.0f;
}

bool MD3Model::load(string modelFilename)
{	
	// Open the file
	ifstream modelFile(modelFilename.c_str(), ios::binary);
	if (!modelFile)
	{
		stringstream str;
		str << "Invalid filename: " << modelFilename.c_str();
		MessageBox(NULL, str.str().c_str(), "Model error", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	// Read the model header into m_header
	modelFile.read(reinterpret_cast<char*>(&m_header), sizeof(MD3Header));

	// Resize data structures according to info supplied in header.
	m_frames.resize(m_header.numFrames);
	m_tags.resize(m_header.numTags * m_header.numFrames);
	m_surfaces.resize(m_header.numSurfaces);

	// Read frames from file.
	modelFile.seekg(m_header.framesOffset);
	modelFile.read(reinterpret_cast<char*>(&m_frames[0]), sizeof(Frame) * m_header.numFrames);

	// Read tags from file.
	if (m_header.numTags > 0)
	{
		modelFile.seekg(m_header.tagsOffset);
		modelFile.read(reinterpret_cast<char*>(&m_tags[0]), sizeof(Tag) * m_header.numTags * m_header.numFrames);
	}

	// Read surfaces from file.
	modelFile.seekg(m_header.surfacesOffset);

	const GLfloat oneOver64 = 1.0f/64.0f;

	for (vector<Surface>::iterator it = m_surfaces.begin();
		 it != m_surfaces.end();
		 ++it)
	{
		// Get current position in file to add to offsets.
		long surfaceStartPos = modelFile.tellg();

		// Read surface header.
		modelFile.read(reinterpret_cast<char*>(&it->header), sizeof(SurfaceHeader));

		// Resize data structures.
		it->shaders.resize(it->header.numShaders);
		it->triangles.resize(it->header.numTris);
		it->texCoords.resize(it->header.numVerts);		
		it->md3vertices.resize(it->header.numVerts * it->header.numFrames);
		it->vertices.resize(it->header.numVerts * it->header.numFrames);
		it->normals.resize(it->header.numVerts * it->header.numFrames);

		// Read data from file.
		modelFile.seekg(surfaceStartPos + it->header.shadersOffset);
		modelFile.read(reinterpret_cast<char*>(&it->shaders[0]), sizeof(Shader) * it->header.numShaders);
		modelFile.seekg(surfaceStartPos + it->header.trisOffset);
		modelFile.read(reinterpret_cast<char*>(&it->triangles[0]), sizeof(Triangle) * it->header.numTris);
		modelFile.seekg(surfaceStartPos + it->header.texCoordsOffset);
		modelFile.read(reinterpret_cast<char*>(&it->texCoords[0]), sizeof(TexCoord) * it->header.numVerts);
		modelFile.seekg(surfaceStartPos + it->header.vertsOffset);
		modelFile.read(reinterpret_cast<char*>(&it->md3vertices[0]), sizeof(MD3Vertex) * it->header.numVerts * it->header.numFrames);

		// Decompress vertex data and create XYZ normals.
		for (unsigned int i = 0; i < it->md3vertices.size(); i++)
		{
			it->vertices[i].v[0] = it->md3vertices[i].coord[0] * oneOver64;
			it->vertices[i].v[1] = it->md3vertices[i].coord[1] * oneOver64;
			it->vertices[i].v[2] = it->md3vertices[i].coord[2] * oneOver64;

			unsigned char zenith = it->md3vertices[i].normal[0];
			unsigned char azimuth = it->md3vertices[i].normal[1];
			GLfloat lat = (GLfloat)zenith * (2.0f * (GLfloat)M_PI) / 255.0f;
			GLfloat lng = (GLfloat)azimuth * (2.0f * (GLfloat)M_PI) / 255.0f;
			it->normals[i].v[0] = cos(lat) * sin(lng);
			it->normals[i].v[1] = sin(lat) * sin(lng);
			it->normals[i].v[2] = cos(lng);
		}
		
		// Flip texcoord t values.
		for (unsigned int i = 0; i < it->texCoords.size(); i++)
			it->texCoords[i].t = 1 - it->texCoords[i].t;
	}
	
	m_vertexBuffers.resize(m_header.numSurfaces);
	m_texCoordBuffers.resize(m_header.numSurfaces);
	m_normalBuffers.resize(m_header.numSurfaces);

	for (int i = 0; i < m_header.numSurfaces; i++)
	{
		// Load texcoord data into buffer.
		glGenBuffers(1, &m_texCoordBuffers[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, m_surfaces[i].header.numVerts * sizeof(TexCoord), &m_surfaces[i].texCoords[0], GL_STATIC_DRAW);

		m_vertexBuffers[i].resize(m_surfaces[i].header.numFrames);		
		m_normalBuffers[i].resize(m_surfaces[i].header.numFrames);

		for (int j = 0; j < m_surfaces[i].header.numFrames; j++)
		{
			int offset = j * m_surfaces[i].header.numVerts;

			// Load vertex data into buffer.
			glGenBuffers(1, &m_vertexBuffers[i][j]);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i][j]);	
			glBufferData(GL_ARRAY_BUFFER, m_surfaces[i].header.numVerts * sizeof(Vector), &m_surfaces[i].vertices[offset], GL_STATIC_DRAW);
			
			// Load vertex normal data into buffer.
			glGenBuffers(1, &m_normalBuffers[i][j]);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[i][j]);
			glBufferData(GL_ARRAY_BUFFER, m_surfaces[i].header.numVerts * sizeof(Vector), &m_surfaces[i].normals[offset], GL_STATIC_DRAW);
		}
	}

	return true;
}

void MD3Model::parseSkinFile(string skinFilename)
{		
	// Open the file
	ifstream skinFile(skinFilename.c_str(), ios::binary);
	if (!skinFile)
	{
		stringstream str;
		str << "Invalid filename: " << skinFilename.c_str();
		MessageBox(NULL, str.str().c_str(), "Skin error", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	std::string tmp;
	while (!skinFile.eof())
	{
		skinFile >> tmp;
		int pos = tmp.find(',');
		if (pos != -1)
		{
			std::string surfaceName = tmp.substr(0, pos);
			std::string texturePath = tmp.substr(pos+1, tmp.length());
			if (!texturePath.empty())
				Utils::loadTexture(texturePath.c_str(), m_textures[surfaceName]);
		}
	}		
}

void MD3Model::addLink(string tagName, MD3Model &model)
{
	m_links[tagName] = &model;
}

void MD3Model::setMaterialProperties(MaterialProps props)
{
	m_materialProps = props;
}

void MD3Model::setAnimationParams(GLuint startFrame, GLuint endFrame, GLuint fps)
{
	m_startFrame = startFrame;
	m_endFrame = endFrame;
	m_fps = fps;

	m_nextFrame = m_startFrame;
	m_animationFinished = false;
}

SimpleBox* MD3Model::getCollider()
{
	// Build bounding box from frame min and max bounds.
	return new SimpleBox(m_frames[m_currentFrame].minBounds.v[0],
						 m_frames[m_currentFrame].maxBounds.v[0],
						 m_frames[m_currentFrame].minBounds.v[1],
						 m_frames[m_currentFrame].maxBounds.v[1],
						 m_frames[m_currentFrame].minBounds.v[2],
						 m_frames[m_currentFrame].maxBounds.v[2]
						 );
}

void MD3Model::onPrepare(float dt)
{
	float oneOverFPS = 1.0f/(float)m_fps;
	// Increment frame if enough time has passed
	m_timeCount += dt;
	if (m_timeCount >= oneOverFPS)
	{
		m_timeCount = fmod(m_timeCount, oneOverFPS);
		m_currentFrame = m_nextFrame;
		m_nextFrame++;
		if (m_nextFrame >= m_endFrame)
		{
			m_animationFinished = true;
			m_nextFrame = m_startFrame;
		}
	}
	m_lerpValue = m_timeCount / oneOverFPS;
	/*stringstream str;
	str << "dt: " << dt << "\tm_timeCount: " << m_timeCount << "\tm_lerpValue: " << m_lerpValue << endl;
	OutputDebugString(str.str().c_str());*/
}

bool MD3Model::hasAnimationFinished()
{
	return m_animationFinished;
}

void MD3Model::drawModel(ShaderProgram *shaderProgram)
{
	for (unsigned int i = 0; i < m_surfaces.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i][m_currentFrame]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i][m_nextFrame]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffers[i]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[i][m_currentFrame]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[i][m_nextFrame]);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(4);
			
		glBindTexture(GL_TEXTURE_2D, m_textures[m_surfaces[i].header.name]);

		shaderProgram->bind();
		shaderProgram->sendMatrices();
		shaderProgram->sendUniform("texture0", 0);		
		shaderProgram->sendUniform("lerp_value", m_lerpValue);
		shaderProgram->sendMaterialProps(m_materialProps);

		glDrawElements(GL_TRIANGLES, m_surfaces[i].triangles.size() * 3, GL_UNSIGNED_INT, &m_surfaces[i].triangles[0]);

		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		//renderNormals(shaderProgram);
	}	
}

/* MD3Model::onRender()
 * Render this model, then iterate through all linked models and render them
 * using information stored in the relevant tag.
 */
void MD3Model::onRender(ShaderProgram *shaderProgram)
{
	drawModel(shaderProgram);
	for (map<string, MD3Model*>::iterator link_it = m_links.begin();
		 link_it != m_links.end();
		 ++link_it)
	{
		// Find the position of the tag we're looking for.
		int tagPos;
		for (int i = 0; i < m_header.numTags; i++)
		{
			if (link_it->first == m_tags[i].name)
			{
				tagPos = i;
				break;
			}
		}

		tagPos += (m_currentFrame * m_header.numTags);

		// Multiply the existing modelview matrix by a matrix constructed
		// from the rotation and translation data specified in the tag.
		GLfloat matrix[16];

		matrix[0] = m_tags[tagPos].axis[0].v[0];
		matrix[1] = m_tags[tagPos].axis[0].v[1];
		matrix[2] = m_tags[tagPos].axis[0].v[2];
		matrix[3] = 0.0f;
		matrix[4] = m_tags[tagPos].axis[1].v[0];
		matrix[5] = m_tags[tagPos].axis[1].v[1];
		matrix[6] = m_tags[tagPos].axis[1].v[2];
		matrix[7] = 0.0f;
		matrix[8] = m_tags[tagPos].axis[2].v[0];
		matrix[9] = m_tags[tagPos].axis[2].v[1];
		matrix[10] = m_tags[tagPos].axis[2].v[2];
		matrix[11] = 0.0f;
		matrix[12] = m_tags[tagPos].origin.v[0];
		matrix[13] = m_tags[tagPos].origin.v[1];
		matrix[14] = m_tags[tagPos].origin.v[2];
		matrix[15] = 1.0f;

		glPushMatrix();
		glMultMatrixf(matrix);
		link_it->second->onRender(shaderProgram);
		glPopMatrix();
	}
}

void MD3Model::renderNormals(ShaderProgram *shaderProgram)
{
	/*glPushMatrix();

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

	glPopMatrix();*/
}

MD3Model::~MD3Model(void)
{
}