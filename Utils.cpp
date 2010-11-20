#include "Utils.h"
#include "targa.h"
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

void Utils::loadTexture(const char* filename, GLuint &texture)
{
	TargaImage textureImage;

	const std::string filename_str(filename);
	//Try open a file containing the serialized object.
	//const std::string bin_filename = filename_str.substr(0, filename_str.find_last_of('.')) + ".bin";
	//std::ifstream binFileIn(bin_filename.c_str(), std::ios::binary);
	//if (!binFileIn.good())
	//{
		if (!textureImage.load(filename))
		{
			MessageBox(NULL, "Could not load texture image", "Texture error", MB_ICONERROR | MB_OK);
		}
		else
		{
			// Serialize the TargaImage to disk to faciliate faster loading next time.
//			std::ofstream binFileOut(bin_filename.c_str(), std::ios::binary);
//			binFileOut.write(reinterpret_cast<char*>(&textureImage), sizeof(textureImage));
//			binFileOut.write(reinterpret_cast<char*>(textureImage.getImageData()), textureImage.getImageSize());
//			binFileOut.close();
		}
	//}
	//else
	//{
		// Read the serialized TargaImage from disk.
	//	binFileIn.read(reinterpret_cast<char*>(&textureImage), sizeof(TargaImage));		
	//	binFileIn.read(reinterpret_cast<char*>(textureImage.getImageData()), textureImage.getImageSize());
	//}

	//binFileIn.close();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (textureImage.getType() == GL_RGB)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, textureImage.getWidth(), textureImage.getHeight(), 
					 0, GL_RGB, GL_UNSIGNED_BYTE ,textureImage.getImageData());
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureImage.getWidth(), textureImage.getHeight(), 
					 0, GL_RGBA, GL_UNSIGNED_BYTE ,textureImage.getImageData());
	textureImage.unload();
}


GLfloat Utils::degreesToRadians(GLfloat degrees)
{
	return degrees * (GLfloat)M_PI / 180.0f;
}

GLfloat Utils::radiansToDegrees(GLfloat radians)
{
	return radians * 180.0f / (GLfloat)M_PI;
}