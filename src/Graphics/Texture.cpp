#include "Texture.h"

namespace wind
{

/******************************************************************************/
Texture::Texture() : _textureID(0), _pixels32(nullptr), _pixels8(nullptr), 
_pixelFormat(0), _imageWidth(0), _imageHeight(0), _textureWidth(0), _textureHeight(0),
_VBOID(0), _IBOID(0)
{
}

/******************************************************************************/
Texture::~Texture()
{
    freeTexture();

    freeVBO();
}

/******************************************************************************/
void Texture::bind(unsigned int unit)
{
    assert(unit >= 0 && unit <= 31);

    glActiveTexture(GL_TEXTURE + unit);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

/******************************************************************************/
void Texture::unbind(unsigned int unit)
{
    assert(unit >= 0 && unit <= 31);

    glActiveTexture(GL_TEXTURE + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/******************************************************************************/
bool Texture::loadTextureFromFile32(const std::string &filePath)
{
    bool loadSuccess = false;

    //This will generate the ID we need for that image.
    ILuint imgID = 0;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    //Here we load the image and check if it worked.
    ILboolean success = ilLoadImage(filePath.c_str());
    if (success == IL_TRUE)
    {
        //Next we need to try and convert the image to RGBA.
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        if (success == IL_TRUE)
        {
            //Here we need to set up the dimensions.
            GLuint imgWidth = (GLuint)ilGetInteger(IL_IMAGE_WIDTH);
            GLuint imgHeight = (GLuint)ilGetInteger(IL_IMAGE_HEIGHT);

            //Next we calculate the texture dimensions.
            GLuint texWidth = powerOfTwo(imgWidth);
            GLuint texHeight = powerOfTwo(imgHeight);

            //Next we need to make sure that the texture is the right side.
            if (imgWidth != texWidth || imgHeight != texHeight)
            {
                //First we need to stick the texture to the top left size and resize it.
                iluImageParameter(ILU_PLACEMENT, ILU_UPPER_LEFT);
                iluEnlargeCanvas(static_cast<int>(texWidth), static_cast<int>(texHeight), 1);
            }

            loadSuccess = loadTextureFromPixels32((GLuint*)ilGetData(), imgWidth, imgHeight, texWidth, texHeight);
        }

        ilDeleteImages(1, &imgID);

        _pixelFormat = GL_RGBA;
    }

    //Finally we tell the users if there is an error.
    if (!loadSuccess)
    {
        std::cerr << "Unable to load: " << filePath << std::endl;
    }

    return loadSuccess;
}

/******************************************************************************/
bool Texture::loadPixelsFromFile32(const std::string &filePath)
{
    freeTexture();

    bool loadSuccess = false;

    //This will generate the ID we need for that image.
    ILuint imgID = 0;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    //Here we load the image and check if it worked.
    ILboolean success = ilLoadImage(filePath.c_str());
    if (success == IL_TRUE)
    {
        //Next we need to try and convert the image to RGBA.
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        if (success == IL_TRUE)
        {
            //Here we need to set up the dimensions.
            GLuint imgWidth = static_cast<GLuint>(ilGetInteger(IL_IMAGE_WIDTH));
            GLuint imgHeight = static_cast<GLuint>(ilGetInteger(IL_IMAGE_HEIGHT));

            //Next we calculate the texture dimensions.
            GLuint texWidth = powerOfTwo(imgWidth);
            GLuint texHeight = powerOfTwo(imgHeight);

            //Next we need to make sure that the texture is the right side.
            if (imgWidth != texWidth || imgHeight != texHeight)
            {
                //First we need to stick the texture to the top left size and resize it.
                iluImageParameter(ILU_PLACEMENT, ILU_UPPER_LEFT);
                iluEnlargeCanvas(static_cast<int>(texWidth), static_cast<int>(texHeight), 1);
            }

            //Next we allocate the data needed for the texture.
            GLuint size = texWidth * texHeight;
            _pixels32 = new GLuint[size];

            //Here we are setting the dimentions of the texture.
            _imageWidth = imgWidth;
            _imageHeight = imgHeight;
            _textureWidth = texWidth;
            _textureHeight = texHeight;

            //Next we memcpy over the data to the pixel8 allocated memory.
            memcpy(_pixels32, ilGetData(), size * 4);

            loadSuccess = true;
        }

        ilDeleteImages(1, &imgID);

        _pixelFormat = GL_RGBA;
    }

    //Finally we tell the users if there is an error.
    if (!loadSuccess)
    {
        std::cerr << "Unable to load: " << filePath << std::endl;
    }

    return loadSuccess;
}

/******************************************************************************/
bool Texture::loadTextureFromFileWithColorKey32(const std::string &filePath, GLubyte r, GLubyte g,
                                                GLubyte b, GLubyte a)
{
    bool success = false;
    if (loadPixelsFromFile32(filePath))
    {
        GLuint size = _textureWidth * _textureHeight;
        for (unsigned int i = 0; i < size; i++)
        {
            GLubyte* colours = (GLubyte*)_pixels32;
            if (colours[0] == r && colours[1] == g && colours[2] == b && (a == 0 || colours[3] == a))
            {
                colours[0] = 225;
                colours[1] = 225;
                colours[2] = 225;
                colours[3] = 0;
            }

        }
    }

    if (loadTextureFromPixels32())
    {
        success = true;
    }

    _pixelFormat = GL_RGBA;

    return success;
}

/******************************************************************************/
bool Texture::loadTextureFromPixels32()
{
    bool success = true;

    if (_textureID == 0 && _pixels32 != NULL)
    {
        glGenTextures(1, &_textureID);

        glBindTexture(GL_TEXTURE_2D, _textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _textureWidth, _textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels32);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cerr << "Error loading texture from " << _pixels32 << 
                         " pixels! " << gluErrorString(error) << std::endl;
            success = false;
        }
        else
        {
            //Here we delete the pixels from memory.
            delete[] _pixels32;
            _pixels32 = nullptr;

            //Then we start up the Vertex Object Buffer.
            initVBO();

            _pixelFormat = GL_RGBA;
        }
    }
    else
    {
        std::cerr << "Cannot load texture from current pixels!" << std::endl;

        //Texture already exists
        if (_textureID != 0)
        {
            std::cerr << "A texture is already loaded!" << std::endl;
        }
        //No pixel loaded
        else if (_pixels32 == nullptr)
        {
            std::cerr << "No pixels to create texture from!" << std::endl;
        }

        success = false;
    }

    return success;
}

/******************************************************************************/
bool Texture::loadTextureCube(std::vector<std::string> filePaths)
{
    bool loadSuccess = false;
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //This will generate the ID we need for that image.
    ILuint imgID = 0;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    for (GLuint i = 0; i < filePaths.size(); i++)
    {
        //Here we load the image and check if it worked.
        ILboolean success = ilLoadImage(filePaths[i].c_str());
        if (success == IL_TRUE)
        {
            //Next we need to try and convert the image to RGBA.
            success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
            if (success == IL_TRUE)
            {
                //Here we need to set up the dimensions.
                GLuint imgWidth = (GLuint)ilGetInteger(IL_IMAGE_WIDTH);
                GLuint imgHeight = (GLuint)ilGetInteger(IL_IMAGE_HEIGHT);

                //Next we calculate the texture dimensions.
                GLuint texWidth = powerOfTwo(imgWidth);
                GLuint texHeight = powerOfTwo(imgHeight);

                //Next we need to make sure that the texture is the right side.
                if (imgWidth != texWidth || imgHeight != texHeight)
                {
                    //First we need to stick the texture to the top left size and resize it.
                    iluImageParameter(ILU_PLACEMENT, ILU_UPPER_LEFT);
                    iluEnlargeCanvas(static_cast<int>(texWidth), static_cast<int>(texHeight), 1);
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLuint*)ilGetData());
            }
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "Error loading texture: " << gluErrorString(error) << std::endl;
        return false;
    }

    _pixelFormat = GL_RGBA;
}

/******************************************************************************/
bool Texture::loadTextureFromPixels32(GLuint *pixels, GLuint imgWidth, GLuint imgHeight,
                                      GLuint texWidth, GLuint texHeight)
{
    freeTexture();

    //bool success = false;
    _imageWidth = imgWidth;
    _imageHeight = imgHeight;
    _textureWidth = texWidth;
    _textureHeight = texHeight;

    glGenTextures(1, &_textureID);

    //Bind texture ID
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _textureWidth, _textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, NULL);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "Error loading texture from" << _pixels32 <<
                     " pixels! " << gluErrorString(error) << std::endl;
        return false;
    }

    //Then we start up the Vertex Object Buffer.
    initVBO();

    _pixelFormat = GL_RGBA;

    return true;
}

/******************************************************************************/
void Texture::createPixels32(GLuint imgWidth, GLuint imgHeight)
{
    if (imgWidth > 0 && imgHeight > 0)
    {
        freeTexture();

        GLuint size = imgWidth * imgHeight;
        _pixels32 = new GLuint[size];
        memset(_pixels32, 0, size * 4);

        _imageWidth = imgWidth;
        _imageHeight = imgHeight;
        _textureWidth = _imageWidth;
        _textureHeight = _imageHeight;

        _pixelFormat = GL_RGBA;
    }
}

/******************************************************************************/
void Texture::copyPixels32(GLuint* pixels, GLuint imgWidth, GLuint imgHeight)
{
    if (imgWidth > 0 && imgHeight > 0)
    {
        freeTexture();

        GLuint size = imgWidth * imgHeight;
        _pixels32 = new GLuint[size];
        memcpy(_pixels32, pixels, size * 4);

        _imageWidth = imgWidth;
        _imageHeight = imgHeight;
        _textureWidth = _imageWidth;
        _textureHeight = _imageHeight;

        _pixelFormat = GL_RGBA;
    }
}

/******************************************************************************/
void Texture::padPixels32()
{
    if (_pixels32 != nullptr)
    {
        //This purely for the memory copying function.
        GLuint oldTextureWidth = _textureWidth;
        //GLuint oldTextureHeight = textureHeight;

        //Here we calculate power of two dimensions
        _textureWidth = powerOfTwo(_imageWidth);
        _textureHeight = powerOfTwo(_imageHeight);

        //Here we check to see if the we need to change the bitmap
        if (_textureWidth != _imageWidth || _textureHeight != _imageHeight)
        {
            //Here we allocated space of the pixels.
            GLuint size = _textureWidth * _textureHeight;
            GLuint* pixels = new GLuint[size];

            //For every pixel row we copy over the data.
            for (unsigned int i = 0; i < _imageHeight; i++)
            {
                memcpy(&pixels[i * _textureWidth], &_pixels32[i * oldTextureWidth], _imageWidth * 4);
            }

            //Finally we change over the pixels.
            delete[] _pixels32;
            _pixels32 = pixels;
        }
    }
}

/******************************************************************************/
bool Texture::loadPixelsFromFile8(std::string filePath)
{
    freeTexture();

    bool loadSuccess = false;

    //This will generate the ID we need for that image.
    ILuint imgID = 0;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    //Here we load the image and check if it worked.
    ILboolean success = ilLoadImage(filePath.c_str());
    if (success == IL_TRUE)
    {
        //Next we need to try and convert the image to gray scale.
        success = ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
        if (success == IL_TRUE)
        {
            //Here we need to set up the dimensions.
            GLuint imgWidth = (GLuint)ilGetInteger(IL_IMAGE_WIDTH);
            GLuint imgHeight = (GLuint)ilGetInteger(IL_IMAGE_HEIGHT);

            //Next we calculate the texture dimensions.
            GLuint texWidth = powerOfTwo(imgWidth);
            GLuint texHeight = powerOfTwo(imgHeight);

            //Next we need to make sure that the texture is the right side.
            if (imgWidth != texWidth || imgHeight != texHeight)
            {
                //First we need to stick the texture to the top left size and resize it.
                iluImageParameter(ILU_PLACEMENT, ILU_UPPER_LEFT);
                iluEnlargeCanvas(static_cast<int>(texWidth), static_cast<int>(texHeight), 1);
            }

            //Next we allocate the data needed for the texture.
            GLuint size = texWidth * texHeight;
            _pixels8 = new GLubyte[size];

            //Here we are setting the dimentions of the texture.
            _imageWidth = imgWidth;
            _imageHeight = imgHeight;
            _textureWidth = texWidth;
            _textureHeight = texHeight;

            //Next we memcpy over the data to the pixel8 allocated memory.
            memcpy(_pixels8, ilGetData(), size);
            loadSuccess = true;
        }
        else
        {
            std::cerr << "Image conversion failed" << std::endl;
        }
        //Delete file from memory
        ilDeleteImages(1, &imgID);

        //Set pixel format
        _pixelFormat = GL_RED;

    }
    else
    {
        std::cerr << "File failed to load" << std::endl;
    }

    //Finally we tell the users if there is an error.
    if (!loadSuccess)
    {
        std::cerr << "Unable to load: " << filePath << std::endl;
    }

    return loadSuccess;
}

/******************************************************************************/
bool Texture::loadTextureFromPixels8()
{
    bool success = true;

    //There is loaded pixels
    if (_textureID == 0 && _pixels8 != NULL)
    {
        //Generate texture ID
        glGenTextures(1, &_textureID);

        //Bind texture ID
        glBindTexture(GL_TEXTURE_2D, _textureID);
        //Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _textureWidth, _textureHeight, 0, 
                     GL_RED, GL_UNSIGNED_BYTE, _pixels8);

        //Generate texture
        glBindTexture(GL_TEXTURE_2D, NULL);

        //Check for error
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cerr << "Error loading texture from " << _pixels8 <<
                          " pixels! " << gluErrorString(error) << std::endl;
            success = false;
        }
        else
        {
            //Release pixels
            delete[] _pixels8;
            _pixels8 = nullptr;

            //Generate VBO
            initVBO();

            //Set pixel format
            _pixelFormat = GL_RED;
        }

    }
    else
    {
        printf("Cannot load texture from current pixels!");

        //Texture already exists
        if (_textureID != 0)
        {
            printf("A texture is already loaded!\n");
        }
        //No pixel loaded
        else if (_pixels8 == nullptr)
        {
            printf("No pixels to create texture from!\n");
        }
    }

    return success;
}

/******************************************************************************/
void Texture::createPixels8(GLuint imgWidth, GLuint imgHeight)
{
    if (imgWidth > 0 && imgHeight > 0)
    {
        freeTexture();

        GLuint size = imgWidth * imgHeight;
        _pixels8 = new GLubyte[size];
        memset(_pixels8, 0, size);

        _imageWidth = imgWidth;
        _imageHeight = imgHeight;
        _textureWidth = _imageWidth;
        _textureHeight = _imageHeight;

        _pixelFormat = GL_RED;
    }
}

/******************************************************************************/
void Texture::copyPixels8(GLubyte* pixels, GLuint imgWidth, GLuint imgHeight)
{
    if (imgWidth > 0 && imgHeight > 0)
    {
        //Get rid of any current texture data
        freeTexture();

        //Copy pixels
        GLuint size = imgWidth * imgHeight;
        _pixels8 = new GLubyte[size];
        memcpy(_pixels8, pixels, size);

        //Copy pixel data
        _imageWidth = imgWidth;
        _imageHeight = imgHeight;
        _textureWidth = _imageWidth;
        _textureHeight = _imageWidth;

        //Set pixel format
        _pixelFormat = GL_RED;
    }
}

/******************************************************************************/
void Texture::padPixels8()
{
    if (_pixels8 != nullptr)
    {
        //Old texture attributes
        GLuint oTextureWidth = _textureWidth;
        GLuint oTextureHeight = _textureHeight;

        //Calculate power of two dimensions
        _textureWidth = powerOfTwo(_imageWidth);
        _textureHeight = powerOfTwo(_imageHeight);

        //The bitmap needs to change
        if (_textureWidth != _imageWidth || _textureHeight != _imageHeight)
        {
            //Allocate pixels
            GLuint size = _textureWidth * _textureHeight;
            GLubyte* pixels = new GLubyte[size];

            //Copy pixels rows
            for (int i = 0; i < _imageHeight; i++)
            {
                memcpy(&pixels[i * _textureWidth], &_pixels8[i * oTextureWidth], _imageWidth);
            }

            //Change pixels
            delete[] _pixels8;
            _pixels8 = pixels;
        }
    }
}

/******************************************************************************/
bool Texture::lock()
{
    bool success = false;
    if (_pixels32 == nullptr && _pixels8 == nullptr && _textureID != 0)
    {
        GLuint size = _textureWidth * _textureHeight;
        if (_pixelFormat == GL_RGBA)
        {
            _pixels32 = new GLuint[size];
        }
        else if (_pixelFormat == GL_RED)
        {
            _pixels8 = new GLubyte[size];
        }

        glBindTexture(GL_TEXTURE_2D, _textureID);

        glGetTexImage(GL_TEXTURE_2D, 0, _pixelFormat, GL_UNSIGNED_BYTE, _pixels32);

        glBindTexture(GL_TEXTURE_2D, 0);

        success = true;
    }

    return success;
}

/******************************************************************************/
template<typename pixels>
bool Texture::unlock()
{
    bool success = false;
    pixels pixy = nullptr;
    if ((_pixels32 != nullptr || _pixels8 != nullptr) && _textureID != 0)
    {
        glBindTexture(GL_TEXTURE_2D, _textureID);

        if (_pixelFormat == GL_RGBA)
        {
            pixy = _pixels32;
        }
        else if (_pixelFormat == GL_RED)
        {
            pixy = _pixels8;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _textureWidth, _textureHeight, _pixelFormat, 
                        GL_UNSIGNED_BYTE, pixy);

        if (_pixels32 != nullptr)
        {
            delete[] _pixels32;
            _pixels32 = nullptr;
        }

        if (_pixels8 != nullptr)
        {
            delete[] _pixels8;
            _pixels8 = nullptr;
        }

        glBindTexture(GL_TEXTURE_2D, NULL);

        success = true;
    }

    return success;
}

/******************************************************************************/
GLuint* Texture::getPixelData32() const
{
    return _pixels32;
}

/******************************************************************************/
GLubyte* Texture::getPixelData8() const
{
    return _pixels8;
}

/******************************************************************************/
GLuint Texture::getPixel32(GLuint x, GLuint y) const
{
    return _pixels32[y * _textureWidth + x];
}

/******************************************************************************/
GLubyte Texture::getPixel8(GLuint x, GLuint y) const
{
    return _pixels8[y * _textureWidth + x];
}

/******************************************************************************/
void Texture::setPixel32(GLuint x, GLuint y, GLuint pixel)
{
    _pixels32[y * _textureWidth + x] = pixel;
}

/******************************************************************************/
void Texture::setPixel8(GLuint x, GLuint y, GLubyte pixel)
{
    _pixels8[y * _textureWidth + x] = pixel;
}

/******************************************************************************/
void Texture::freeTexture()
{
    if (_textureID != 0)
    {
        glDeleteTextures(1, &_textureID);
        _textureID = 0;
    }

    //Delete 32bit pixels
    if (_pixels32 != nullptr)
    {
        delete[] _pixels32;
        _pixels32 = nullptr;
    }

    //Delete 8bit pixels
    if (_pixels8 != nullptr)
    {
        delete[] _pixels8;
        _pixels8 = nullptr;
    }

    _imageWidth = 0;
    _imageHeight = 0;
    _textureWidth = 0;
    _textureHeight = 0;

    //Set pixel format
    _pixelFormat = 0;
}

/******************************************************************************/
GLuint Texture::powerOfTwo(GLuint number) const
{
    if (number != 0)
    {
        number--;
        number |= (number >> 1); //Or first 2 bits
        number |= (number >> 2); //Or next 2 bits
        number |= (number >> 4); //Or next 4 bits
        number |= (number >> 8); //Or next 8 bits
        number |= (number >> 16); //Or next 16 bits
        number++;
    }

    return number;
}

/******************************************************************************/
void Texture::blitPixels32(GLuint x, GLuint y, Texture& destination)
{
    if (_pixels32 != nullptr && destination._pixels32 != nullptr)
    {
        GLuint* dPixel = destination._pixels32;
        GLuint* sPixel = _pixels32;
        for (unsigned int i = 0; i < _imageHeight; i++)
        {
            memcpy(&dPixel[(i + y) * destination._textureWidth + x], &sPixel[i * _textureWidth], _imageWidth * 4);
        }
    }
}

/******************************************************************************/
void Texture::blitPixels8(GLuint x, GLuint y, Texture& destination)
{
    if (_pixels8 != NULL && destination._pixels8 != NULL)
    {
        //Copy pixels rows
        GLubyte* dPixels = destination._pixels8;
        GLubyte* sPixels = _pixels8;
        for (unsigned int i = 0; i < _imageHeight; i++)
        {
            memcpy(&dPixels[(i + y) * destination._textureWidth + x], &sPixels[i * _textureWidth], _imageWidth);
        }
    }
}

/******************************************************************************/
GLuint Texture::getTextureID() const
{
    return _textureID;
}

/******************************************************************************/
GLuint Texture::getImageWidth() const
{
    return _imageWidth;
}

/******************************************************************************/
GLuint Texture::getImageHeight() const
{
    return _imageHeight;
}

/******************************************************************************/
GLuint Texture::getTextureWidth() const
{
    return _textureWidth;
}

/******************************************************************************/
GLuint Texture::getTextureHeight() const
{
    return _textureHeight;
}

/******************************************************************************/
void Texture::initVBO()
{
    if (_textureID != 0 && _VBOID == 0)
    {
        //Vertex data
        TextureVertex2D vData[4];
        GLuint iData[4];

        //Set rendering indices
        iData[0] = 0;
        iData[1] = 1;
        iData[2] = 2;
        iData[3] = 3;

        //Create VBO
        glGenBuffers(1, &_VBOID);
        glBindBuffer(GL_ARRAY_BUFFER, _VBOID);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(TextureVertex2D), vData, GL_STATIC_DRAW);

        //Create IBO
        glGenBuffers(1, &_IBOID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBOID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), iData, GL_STATIC_DRAW);

        //Unbind buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

/******************************************************************************/
void Texture::freeVBO()
{
    if (_VBOID != 0)
    {
        glDeleteBuffers(1, &_VBOID);
        glDeleteBuffers(1, &_IBOID);
    }
}
};