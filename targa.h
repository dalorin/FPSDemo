/* TARGA library from Beginning OpenGL Game Programming Second Edition - Chapter 13 *
 * Library by Benstead, Astle and Hawkins                                           */

#ifndef TARGA_HEADER_H
#define TARGA_HEADER_H

#include <vector>
#include <string>
#include <fstream>
#include "glee.h"

// These masks are AND'd with the imageDesc in the TGA header,
// bit 4 is left-to-right ordering
// bit 5 is top-to-bottom
enum IMAGE_ORIENTATIONS {
    BOTTOM_LEFT = 0x00,	// first pixel is bottom left corner
    BOTTOM_RIGHT = 0x10,	// first pixel is bottom right corner
    TOP_LEFT = 0x20,	// first pixel is top left corner
    TOP_RIGHT = 0x30	// first pixel is top right corner
};

struct TargaHeader
{
    unsigned char idLength;
    unsigned char colorMapType;
    unsigned char imageTypeCode;
    unsigned char colorMapSpec[5];
    unsigned short xOrigin;
    unsigned short yOrigin;
    unsigned short width;
    unsigned short height;
    unsigned char bpp;
    unsigned char imageDesc;
};

enum TargaFileTypes
{
    TFT_NO_DATA = 0,
    TFT_INDEXED = 1,
    TFT_RGB = 2,
    TFT_GRAYSCALE = 3,
    TFT_RLE_INDEXED = 9,
    TFT_RLE_RGB = 10,
    TFT_RLE_GRAYSCALE = 11
};

class TargaImage
{
public:
    TargaImage();
    virtual ~TargaImage();

    bool load(const std::string& filename);
    void unload();

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getBitsPerPixel() const;
    const unsigned char* getImageData() const;
	unsigned int getImageSize() const;

	void resizeImageData(unsigned int size);

    GLenum getType() const {
        unsigned int bits = getBitsPerPixel();
        if (bits <= 24) {
            return GL_RGB;
        }
        if (bits == 32) {
            return GL_RGBA;
        }
    }
private:
    TargaHeader m_header;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bitsPerPixel;
    unsigned int m_bytesPerPixel;

    std::vector<unsigned char> m_imageData;

    bool loadUncompressedTarga(std::istream& fileIn);
    bool loadCompressedTarga(std::istream& fileIn);

    bool isImageTypeSupported(const TargaHeader& header);
    bool isCompressedTarga(const TargaHeader& header);
    bool isUncompressedTarga(const TargaHeader& header);

    void flipImageVertically();
};

#endif
