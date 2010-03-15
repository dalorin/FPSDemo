#include "Utils.h"
#include "targa.h"

void Utils::loadTexture(const char* filename, GLuint &texture)
{
	TargaImage textureImage;
	if (!textureImage.load(filename))
	{
		MessageBox(NULL, "Could not load texture image", "Texture error", MB_ICONERROR | MB_OK);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, textureImage.getWidth(), textureImage.getHeight(), 
				 0, GL_RGB, GL_UNSIGNED_BYTE ,textureImage.getImageData());
	textureImage.unload();
}