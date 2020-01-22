#include "Texture.h"

namespace wind
{
Texture::Texture()
{
	textureID = 0;
	pixels32 = nullptr;
	pixels8 = nullptr;
	pixelFormat = 0;

	imageWidth = 0;
	imageHeight = 0;

	textureWidth = 0;
	textureHeight = 0;

	VBOID = 0;
	IBOID = 0;
}

Texture::Texture(const std::string& filepath) : imageData(nullptr)
{
	textureID = 0;
	pixels32 = NULL;
	pixels8 = NULL;
	pixelFormat = 0;

	imageWidth = 0;
	imageHeight = 0;

	textureWidth = 0;
	textureHeight = 0;

	VBOID = 0;
	IBOID = 0;
}

Texture::~Texture()
{
	freeTexture();

	freeVBO();
}

void Texture::bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::loadTextureFromFile32(std::string filePath)
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

		pixelFormat = GL_RGBA;
	}

	//Finally we tell the users if there is an error.
	if (!loadSuccess)
	{
		std::cerr << "Unable to load: " << filePath << std::endl;
	}

	return loadSuccess;
}

bool Texture::loadPixelsFromFile32(std::string filePath)
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
			pixels32 = new GLuint[size];

			//Here we are setting the dimentions of the texture.
			imageWidth = imgWidth;
			imageHeight = imgHeight;
			textureWidth = texWidth;
			textureHeight = texHeight;

			//Next we memcpy over the data to the pixel8 allocated memory.
			memcpy(pixels32, ilGetData(), size * 4);

			loadSuccess = true;
		}

		ilDeleteImages(1, &imgID);

		pixelFormat = GL_RGBA;
	}

	//Finally we tell the users if there is an error.
	if (!loadSuccess)
	{
		std::cerr << "Unable to load: " << filePath << std::endl;
	}

	return loadSuccess;
}

bool Texture::loadTextureFromFileWithColorKey32(std::string filePath, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	if (!loadPixelsFromFile32(filePath))
	{
		return false;
	}

	GLuint size = textureWidth * textureHeight;
	for (unsigned int i = 0; i < size; i++)
	{
		GLubyte* colours = (GLubyte*)pixels32;
		if (colours[0] == r && colours[1] == g && colours[2] == b && (a == 0 || colours[3] == a))
		{
			colours[0] = 225;
			colours[1] = 225;
			colours[2] = 225;
			colours[3] = 0;
		}

	}

	if (!loadTextureFromPixels32())
	{
		return false;
	}

	pixelFormat = GL_RGBA;

	return true;
}

bool Texture::loadTextureFromPixels32()
{
	bool success = true;

	if (textureID == 0 && pixels32 != NULL)
	{
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels32);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "Error loading texture from " << pixels32 << " pixels! " << gluErrorString(error) << std::endl;
			success = false;
		}
		else
		{
			//Here we delete the pixels from memory.
			delete[] pixels32;
			pixels32 = nullptr;

			//Then we start up the Vertex Object Buffer.
			initVBO();

			pixelFormat = GL_RGBA;
		}
	}
	else
	{
		std::cerr << "Cannot load texture from current pixels!" << std::endl;

		//Texture already exists
		if (textureID != 0)
		{
			std::cerr << "A texture is already loaded!" << std::endl;
		}
		//No pixel loaded
		else if (pixels32 == nullptr)
		{
			std::cerr << "No pixels to create texture from!" << std::endl;
		}

		success = false;
	}

	return success;
}

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

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLuint*)ilGetData());
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

	pixelFormat = GL_RGBA;
}

bool Texture::loadTextureFromPixels32(GLuint* pixels, GLuint imgWidth, GLuint imgHeight, GLuint texWidth, GLuint texHeight)
{
	freeTexture();

	//bool success = false;
	imageWidth = imgWidth;
	imageHeight = imgHeight;
	textureWidth = texWidth;
	textureHeight = texHeight;

	glGenTextures(1, &textureID);

	//Bind texture ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, NULL);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "Error loading texture from" << pixels32 << " pixels! " << gluErrorString(error) << std::endl;
		return false;
	}

	//Then we start up the Vertex Object Buffer.
	initVBO();

	pixelFormat = GL_RGBA;

	return true;
}

void Texture::createPixels32(GLuint imgWidth, GLuint imgHeight)
{
	if (imgWidth > 0 && imgHeight > 0)
	{
		freeTexture();

		GLuint size = imgWidth * imgHeight;
		pixels32 = new GLuint[size];
		memset(pixels32, 0, size * 4);

		imageWidth = imgWidth;
		imageHeight = imgHeight;
		textureWidth = imageWidth;
		textureHeight = imageHeight;

		pixelFormat = GL_RGBA;
	}
}

void Texture::copyPixels32(GLuint* pixels, GLuint imgWidth, GLuint imgHeight)
{
	if (imgWidth > 0 && imgHeight > 0)
	{
		freeTexture();

		GLuint size = imgWidth * imgHeight;
		pixels32 = new GLuint[size];
		memcpy(pixels32, pixels, size * 4);

		imageWidth = imgWidth;
		imageHeight = imgHeight;
		textureWidth = imageWidth;
		textureHeight = imageHeight;

		pixelFormat = GL_RGBA;
	}
}

void Texture::padPixels32()
{
	if (pixels32 != nullptr)
	{
		//This purely for the memory copying function.
		GLuint oldTextureWidth = textureWidth;
		//GLuint oldTextureHeight = textureHeight;

		//Here we calculate power of two dimensions
		textureWidth = powerOfTwo(imageWidth);
		textureHeight = powerOfTwo(imageHeight);

		//Here we check to see if the we need to change the bitmap
		if (textureWidth != imageWidth || textureHeight != imageHeight)
		{
			//Here we allocated space of the pixels.
			GLuint size = textureWidth * textureHeight;
			GLuint* pixels = new GLuint[size];

			//For every pixel row we copy over the data.
			for (unsigned int i = 0; i < imageHeight; i++)
			{
				memcpy(&pixels[i * textureWidth], &pixels32[i * oldTextureWidth], imageWidth * 4);
			}

			//Finally we change over the pixels.
			delete[] pixels32;
			pixels32 = pixels;
		}
	}
}

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
			pixels8 = new GLubyte[size];

			//Here we are setting the dimentions of the texture.
			imageWidth = imgWidth;
			imageHeight = imgHeight;
			textureWidth = texWidth;
			textureHeight = texHeight;

			//Next we memcpy over the data to the pixel8 allocated memory.
			memcpy(pixels8, ilGetData(), size);
			loadSuccess = true;
		}
		else
		{
			std::cerr << "Image conversion failed" << std::endl;
		}
		//Delete file from memory
		ilDeleteImages(1, &imgID);

		//Set pixel format
		pixelFormat = GL_RED;

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

bool Texture::loadTextureFromPixels8()
{
	bool success = true;

	//There is loaded pixels
	if (textureID == 0 && pixels8 != NULL)
	{
		//Generate texture ID
		glGenTextures(1, &textureID);

		//Bind texture ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		//Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels8);

		//Generate texture
		glBindTexture(GL_TEXTURE_2D, NULL);

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "Error loading texture from " << pixels8 << " pixels! " << gluErrorString(error) << std::endl;
			success = false;
		}
		else
		{
			//Release pixels
			delete[] pixels8;
			pixels8 = NULL;

			//Generate VBO
			initVBO();

			//Set pixel format
			pixelFormat = GL_RED;
		}

	}
	else
	{
		printf("Cannot load texture from current pixels!");

		//Texture already exists
		if (textureID != 0)
		{
			printf("A texture is already loaded!\n");
		}
		//No pixel loaded
		else if (pixels8 == NULL)
		{
			printf("No pixels to create texture from!\n");
		}
	}

	return success;
}

void Texture::createPixels8(GLuint imgWidth, GLuint imgHeight)
{
	if (imgWidth > 0 && imgHeight > 0)
	{
		freeTexture();

		GLuint size = imgWidth * imgHeight;
		pixels8 = new GLubyte[size];
		memset(pixels8, 0, size);

		imageWidth = imgWidth;
		imageHeight = imgHeight;
		textureWidth = imageWidth;
		textureHeight = imageHeight;

		pixelFormat = GL_RED;
	}
}

void Texture::copyPixels8(GLubyte* pixels, GLuint imgWidth, GLuint imgHeight)
{
	if (imgWidth > 0 && imgHeight > 0)
	{
		//Get rid of any current texture data
		freeTexture();

		//Copy pixels
		GLuint size = imgWidth * imgHeight;
		pixels8 = new GLubyte[size];
		memcpy(pixels8, pixels, size);

		//Copy pixel data
		imageWidth = imgWidth;
		imageHeight = imgHeight;
		textureWidth = imageWidth;
		textureHeight = imageWidth;

		//Set pixel format
		pixelFormat = GL_RED;
	}
}

void Texture::padPixels8()
{
	if (pixels8 != NULL)
	{
		//Old texture attributes
		GLuint oTextureWidth = textureWidth;
		GLuint oTextureHeight = textureHeight;

		//Calculate power of two dimensions
		textureWidth = powerOfTwo(imageWidth);
		textureHeight = powerOfTwo(imageHeight);

		//The bitmap needs to change
		if (textureWidth != imageWidth || textureHeight != imageHeight)
		{
			//Allocate pixels
			GLuint size = textureWidth * textureHeight;
			GLubyte* pixels = new GLubyte[size];

			//Copy pixels rows
			for (int i = 0; i < imageHeight; i++)
			{
				memcpy(&pixels[i * textureWidth], &pixels8[i * oTextureWidth], imageWidth);
			}

			//Change pixels
			delete[] pixels8;
			pixels8 = pixels;
		}
	}
}

bool Texture::lock()
{
	if (pixels32 == nullptr && pixels8 == nullptr && textureID != 0)
	{
		GLuint size = textureWidth * textureHeight;
		if (pixelFormat == GL_RGBA)
		{
			pixels32 = new GLuint[size];
		}
		else if (pixelFormat == GL_RED)
		{
			pixels8 = new GLubyte[size];
		}

		glBindTexture(GL_TEXTURE_2D, textureID);

		glGetTexImage(GL_TEXTURE_2D, 0, pixelFormat, GL_UNSIGNED_BYTE, pixels32);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	return false;
}

template<typename pixels>
bool Texture::unlock()
{
	pixels pixy = nullptr;
	if ((pixels32 != nullptr || pixels8 != nullptr) && textureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (pixelFormat == GL_RGBA)
		{
			pixy = pixels32;
		}
		else if (pixelFormat == GL_RED)
		{
			pixy = pixels8;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, pixelFormat, GL_UNSIGNED_BYTE, pixy);

		if (pixels32 != nullptr)
		{
			delete[] pixels32;
			pixels32 = nullptr;
		}
		if (pixels8 != nullptr)
		{
			delete[] pixels8;
			pixels8 = nullptr;
		}

		glBindTexture(GL_TEXTURE_2D, NULL);

		return true;
	}

	return false;
}

GLuint* Texture::getPixelData32()
{
	return pixels32;
}

GLubyte* Texture::getPixelData8()
{
	return pixels8;
}

GLuint Texture::getPixel32(GLuint x, GLuint y)
{
	return pixels32[y * textureWidth + x];
}

GLubyte Texture::getPixel8(GLuint x, GLuint y)
{
	return pixels8[y * textureWidth + x];
}

void Texture::setPixel32(GLuint x, GLuint y, GLuint pixel)
{
	pixels32[y * textureWidth + x] = pixel;
}

void Texture::setPixel8(GLuint x, GLuint y, GLubyte pixel)
{
	pixels8[y * textureWidth + x] = pixel;
}

void Texture::freeTexture()
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}

	//Delete 32bit pixels
	if (pixels32 != nullptr)
	{
		delete[] pixels32;
		pixels32 = nullptr;
	}

	//Delete 8bit pixels
	if (pixels8 != nullptr)
	{
		delete[] pixels8;
		pixels8 = nullptr;
	}

	imageWidth = 0;
	imageHeight = 0;
	textureWidth = 0;
	textureHeight = 0;

	//Set pixel format
	pixelFormat = 0;
}

GLuint Texture::powerOfTwo(GLuint number)
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

void Texture::blitPixels32(GLuint x, GLuint y, Texture& destination)
{
	if (pixels32 != nullptr && destination.pixels32 != nullptr)
	{
		GLuint* dPixel = destination.pixels32;
		GLuint* sPixel = pixels32;
		for (unsigned int i = 0; i < imageHeight; i++)
		{
			memcpy(&dPixel[(i + y) * destination.textureWidth + x], &sPixel[i * textureWidth], imageWidth * 4);
		}
	}
}

void Texture::blitPixels8(GLuint x, GLuint y, Texture& destination)
{
	if (pixels8 != NULL && destination.pixels8 != NULL)
	{
		//Copy pixels rows
		GLubyte* dPixels = destination.pixels8;
		GLubyte* sPixels = pixels8;
		for (unsigned int i = 0; i < imageHeight; i++)
		{
			memcpy(&dPixels[(i + y) * destination.textureWidth + x], &sPixels[i * textureWidth], imageWidth);
		}
	}
}

GLuint Texture::getTextureID()
{
	return textureID;
}

GLuint Texture::getImageWidth()
{
	return imageWidth;
}

GLuint Texture::getImageHeight()
{
	return imageHeight;
}

GLuint Texture::getTextureWidth()
{
	return textureWidth;
}

GLuint Texture::getTextureHeight()
{
	return textureHeight;
}

void Texture::initVBO()
{
	if (textureID != 0 && VBOID == 0)
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
		glGenBuffers(1, &VBOID);
		glBindBuffer(GL_ARRAY_BUFFER, VBOID);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(TextureVertex2D), vData, GL_STATIC_DRAW);

		//Create IBO
		glGenBuffers(1, &IBOID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), iData, GL_STATIC_DRAW);

		//Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Texture::freeVBO()
{
	if (VBOID != 0)
	{
		glDeleteBuffers(1, &VBOID);
		glDeleteBuffers(1, &IBOID);
	}
}
};