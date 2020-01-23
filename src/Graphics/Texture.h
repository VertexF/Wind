#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cstring>

#include "include/TextureVertex2D.h"
#include "include/FontRect.h"

namespace wind
{
/**
    In this class we are loading textures using some external code.
    I have made it so it can be used as an instance variable so you can run the constructor or load the file by a function.
*/
class Texture
{
public:
    Texture();
    virtual ~Texture();

    //This also returns a copy of the textureID.
    //Note: If it is failing pass it as a pointer rather than just stack.
    virtual void bind(unsigned int unit);
    void unbind(unsigned int unit);

    //This function will be able to load any image with DevIL.
    bool loadTextureFromFile32(const std::string &filePath);
    bool loadPixelsFromFile32(const std::string &filePath);

    bool loadTextureFromFileWithColorKey32(const std::string &filePath, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 000);
    bool loadTextureFromPixels32();
    bool loadTextureFromPixels32(GLuint* pixels, GLuint imgWidth, GLuint imgHeight, GLuint texWidth, GLuint texHeight);

    bool loadTextureCube(std::vector<std::string> filePaths);

    void createPixels32(GLuint imgWidth, GLuint imgHeight);
    void copyPixels32(GLuint* pixels, GLuint imgWidth, GLuint imgHeight);
    void padPixels32();

    bool loadPixelsFromFile8(std::string filePath);
    bool loadTextureFromPixels8();

    void createPixels8(GLuint imgWidth, GLuint imgHeight);
    void copyPixels8(GLubyte* pixels, GLuint imgWidth, GLuint imgHeight);
    void padPixels8();

    //This function effective just allocates space for a texture.
    bool lock();
    //This function does the reverse of the function above.
    template<typename pixels>
    bool unlock();

    GLuint* getPixelData32() const;
    GLubyte* getPixelData8() const;

    GLuint getPixel32(GLuint x, GLuint y) const;
    GLubyte getPixel8(GLuint x, GLuint y) const;

    void setPixel32(GLuint x, GLuint y, GLuint pixel);
    void setPixel8(GLuint x, GLuint y, GLubyte pixel);

    void blitPixels32(GLuint x, GLuint y, Texture& destination);
    void blitPixels8(GLuint x, GLuint y, Texture& destination);

    GLuint getTextureID() const;
    GLuint getImageWidth() const;
    GLuint getImageHeight() const;
    GLuint getTextureWidth() const;
    GLuint getTextureHeight() const;

    void freeTexture();
    void initVBO();

protected:
    GLuint powerOfTwo(GLuint number) const;
    void freeVBO();
private:
    GLuint _textureID;

    //All data holds parts of the image data.
    //Current pixels
    GLuint *_pixels32;
    GLubyte *_pixels8;

    //Pixel format
    GLuint _pixelFormat;

    //Texture dimensions
    GLuint _textureWidth;
    GLuint _textureHeight;

    //Unpadded image dimensions
    GLuint _imageWidth;
    GLuint _imageHeight;

    //VBO IDs
    GLuint _VBOID;
    GLuint _IBOID;
};
}; //wind
#endif
