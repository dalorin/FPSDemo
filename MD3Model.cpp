#include "MD3Model.h"
#include <fstream>
#include <sstream>
#include "targa.h"
#include "Shader.h"
#include "Utils.h"
#include <map>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

MD3Model::MD3Model(const char* modelPath, const char* textureFilename)
{
	load(modelPath, textureFilename);	
}

bool MD3Model::load(const char* modelPath, const char* textureFilename)
{
	m_components.push_back("lower");
	m_components.push_back("upper");
	m_components.push_back("head");

	for (vector<const char*>::iterator component_it = m_components.begin();
		 component_it != m_components.end();
		 ++component_it)
	{
		const char* component = *component_it;
		// Construct the filename
		stringstream str;
		str << modelPath << component << ".md3";
		std::string filename = str.str();

		// Open the file
		ifstream modelFile(filename.c_str(), ios::binary);
		if (!modelFile)
		{
			stringstream str;
			str << "Invalid filename: " << filename;
			MessageBox(NULL, str.str().c_str(), "Model error", MB_ICONERROR | MB_OK);
			exit(-1);
		}

		// Read the model header into m_header
		modelFile.read(reinterpret_cast<char*>(&m_header), sizeof(MD3Header));

		// Resize data structures according to info supplied in header.
		m_frames[component].resize(m_header.numFrames);
		m_tags[component].resize(m_header.numTags);
		m_surfaces[component].resize(m_header.numSurfaces);

		// Read frames from file.
		modelFile.seekg(m_header.framesOffset);
		modelFile.read(reinterpret_cast<char*>(&m_frames[component][0]), sizeof(Frame) * m_header.numFrames);

		// Read tags from file.
		if (m_header.numTags > 0)
		{
			modelFile.seekg(m_header.tagsOffset);
			modelFile.read(reinterpret_cast<char*>(&m_tags[component][0]), sizeof(Tag) * m_header.numTags);
		}

		// Read surfaces from file.
		modelFile.seekg(m_header.surfacesOffset);

		const GLfloat oneOver64 = 1.0f/64.0f;

		for (vector<Surface>::iterator it = m_surfaces[component].begin();
			 it != m_surfaces[component].end();
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
		
		m_vertexBuffers[component].resize(m_header.numSurfaces);
		m_texCoordBuffers[component].resize(m_header.numSurfaces);
		m_normalBuffers[component].resize(m_header.numSurfaces);

		for (int i = 0; i < m_header.numSurfaces; i++)
		{
			// Load vertex data into buffer.
			glGenBuffers(1, &m_vertexBuffers[component][i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[component][i]);	
			glBufferData(GL_ARRAY_BUFFER, m_surfaces[component][i].vertices.size() * sizeof(Vector), &m_surfaces[component][i].vertices[0], GL_DYNAMIC_DRAW);

			// Load texcoord data into buffer.
			glGenBuffers(1, &m_texCoordBuffers[component][i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffers[component][i]);
			glBufferData(GL_ARRAY_BUFFER, m_surfaces[component][i].texCoords.size() * sizeof(TexCoord), &m_surfaces[component][i].texCoords[0], GL_DYNAMIC_DRAW);
			
			// Load vertex normal data into buffer.
			glGenBuffers(1, &m_normalBuffers[component][i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[component][i]);
			glBufferData(GL_ARRAY_BUFFER, m_surfaces[component][i].normals.size() * sizeof(Vector), &m_surfaces[component][i].normals[0], GL_DYNAMIC_DRAW);			
		}
	}

	parseSkins(modelPath);

	return true;
}

void MD3Model::parseSkins(const char* modelPath)
{
	for (vector<const char*>::iterator component_it = m_components.begin();
		 component_it != m_components.end();
		 ++component_it)
	{
		const char* component = *component_it;
		// Construct the filename
		stringstream str;
		str << modelPath << component << "_default.skin";
		std::string filename = str.str();

		// Open the file
		ifstream skinFile(filename.c_str(), ios::binary);
		if (!skinFile)
		{
			stringstream str;
			str << "Invalid filename: " << filename;
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
}

void MD3Model::setMaterialProperties(MaterialProps props)
{
	m_materialProps = props;
}

void MD3Model::onPrepare(float dt)
{
	//m_interpolatedKeyFrame = m_keyframes[0];
}

void MD3Model::onRender(ShaderProgram *shaderProgram)
{
	glPushMatrix();
	
	Tag *tag = NULL;

	for (vector<const char*>::iterator component_it = m_components.begin();
		 component_it != m_components.end();
		 ++component_it)
	{
		const char* component = *component_it;
		
		for (unsigned int i = 0; i < m_surfaces[component].size(); i++)
		{
			//glPushMatrix();

			if (tag != NULL)
			{
				Vector *v = &tag->origin;
				GLfloat matrix[16];				
				matrix[0] = tag->axis[0].v[0];
				matrix[1] = tag->axis[0].v[1];
				matrix[2] = tag->axis[0].v[2];
				matrix[3] = 0.0f;
				matrix[4] = tag->axis[1].v[0];
				matrix[5] = tag->axis[1].v[1];
				matrix[6] = tag->axis[1].v[2];
				matrix[7] = 0.0f;
				matrix[8] = tag->axis[2].v[0];
				matrix[9] = tag->axis[2].v[1];
				matrix[10] = tag->axis[2].v[2];
				matrix[11] = 0.0f;
				matrix[12] = v->v[0];
				matrix[13] = v->v[1];
				matrix[14] = v->v[2];
				matrix[15] = 1.0f;

				glMultMatrixf(matrix);
				tag = NULL;
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[component][i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_surfaces[component][i].header.numVerts / m_header.numFrames));
			glEnableVertexAttribArray(0);
			
			glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffers[component][i]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_surfaces[component][i].header.numVerts / m_header.numFrames));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[component][i]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_surfaces[component][i].header.numVerts / m_header.numFrames));
			glEnableVertexAttribArray(2);
				
			glBindTexture(GL_TEXTURE_2D, m_textures[m_surfaces[component][i].header.name]);

			shaderProgram->sendMatrices();
			shaderProgram->sendUniform("texture0", 0);
			shaderProgram->sendMaterialProps(m_materialProps);

			glDrawElements(GL_TRIANGLES, m_surfaces[component][i].triangles.size() * 3, GL_UNSIGNED_INT, &m_surfaces[component][i].triangles[0]);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);

			//renderNormals(shaderProgram);
			
			//XXX: This bit is total shit. Fix it.
			if (strcmp(component, "lower") == 0 || strcmp(component, "upper") == 0)
			{
				tag = &m_tags[component][0];
			}

			//glPopMatrix();
		}
	}
			
	glPopMatrix();
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