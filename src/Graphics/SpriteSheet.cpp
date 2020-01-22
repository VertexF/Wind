#include "SpriteSheet.h"

namespace wind
{
/******************************************************************************/
//Starting off we need to set everything to null
SpriteSheet::SpriteSheet() : _vertexDataBuffer(0), _indexBuffers(nullptr)
{
}

/******************************************************************************/
SpriteSheet::~SpriteSheet()
{
	freeSheet();
}

/******************************************************************************/
void SpriteSheet::bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE + unit);
	glBindTexture(GL_TEXTURE_2D, getTextureID());
}

/******************************************************************************/
int SpriteSheet::addClipSprite(const FontRect& newClip)
{
	_clip.push_back(newClip);
	return _clip.size() - 1;
}

/******************************************************************************/
FontRect SpriteSheet::getRect(int index) const
{
	return _clip[index];
}

/******************************************************************************/
bool SpriteSheet::generateDataBuffer(SpriteOrigin origin)
{
	//If there is a texture loaded and clips to make vertex data from
	if (getTextureID() != 0 && _clip.size() > 0)
	{
		//Allocate vertex data
		int totalSprites = _clip.size();
		TextureVertex2D* vertexData = new TextureVertex2D[totalSprites * 4];
		_indexBuffers = new GLuint[totalSprites];

		//Allocate vertex data buffer name
		glGenBuffers(1, &_vertexDataBuffer);

		//Allocate index buffers names
		glGenBuffers(totalSprites, _indexBuffers);

		//Go through clips
		GLfloat tW = getTextureWidth();
		GLfloat tH = getTextureHeight();
		GLuint spriteIndices[4] = { 0, 0, 0, 0 };

		//Origin variables
		GLfloat vTop = 0.f;
		GLfloat vBottom = 0.f;
		GLfloat vLeft = 0.f;
		GLfloat vRight = 0.f;

		for (int i = 0; i < totalSprites; i++)
		{
			//Initialize indices
			spriteIndices[0] = i * 4 + 0;
			spriteIndices[1] = i * 4 + 1;
			spriteIndices[2] = i * 4 + 2;
			spriteIndices[3] = i * 4 + 3;

			//Set origin
			switch (origin)
			{
			case SPRITE_ORIGIN_TOP_LEFT:
				vTop = 0.f;
				vBottom = _clip[i].h;
				vLeft = 0.f;
				vRight = _clip[i].w;
				break;

			case SPRITE_ORIGIN_TOP_RIGHT:
				vTop = 0.f;
				vBottom = _clip[i].h;
				vLeft = -_clip[i].w;
				vRight = 0.f;
				break;

			case SPRITE_ORIGIN_BOTTOM_LEFT:
				vTop = -_clip[i].h;
				vBottom = 0.f;
				vLeft = 0.f;
				vRight = _clip[i].w;
				break;

			case SPRITE_ORIGIN_BOTTOM_RIGHT:
				vTop = -_clip[i].h;
				vBottom = 0.f;
				vLeft = -_clip[i].w;
				vRight = 0.f;
				break;

				//Also for LSPRITE_ORIGIN_CENTER
			default:
				vTop = -_clip[i].h / 2.f;
				vBottom = _clip[i].h / 2.f;
				vLeft = -_clip[i].w / 2.f;
				vRight = _clip[i].w / 2.f;
				break;
			}

			//Top left
			vertexData[spriteIndices[0]].pos.x = vLeft;
			vertexData[spriteIndices[0]].pos.y = vTop;

			vertexData[spriteIndices[0]].texCoord.s = (_clip[i].x) / tW;
			vertexData[spriteIndices[0]].texCoord.t = (_clip[i].y) / tH;

			//Top right
			vertexData[spriteIndices[1]].pos.x = vRight;
			vertexData[spriteIndices[1]].pos.y = vTop;

			vertexData[spriteIndices[1]].texCoord.s = (_clip[i].x + _clip[i].w) / tW;
			vertexData[spriteIndices[1]].texCoord.t = (_clip[i].y) / tH;

			//Bottom right
			vertexData[spriteIndices[2]].pos.x = vRight;
			vertexData[spriteIndices[2]].pos.y = vBottom;

			vertexData[spriteIndices[2]].texCoord.s = (_clip[i].x + _clip[i].w) / tW;
			vertexData[spriteIndices[2]].texCoord.t = (_clip[i].y + _clip[i].h) / tH;

			//Bottom left
			vertexData[spriteIndices[3]].pos.x = vLeft;
			vertexData[spriteIndices[3]].pos.y = vBottom;

			vertexData[spriteIndices[3]].texCoord.s = (_clip[i].x) / tW;
			vertexData[spriteIndices[3]].texCoord.t = (_clip[i].y + _clip[i].h) / tH;

			//Bind sprite index buffer data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), spriteIndices, GL_STATIC_DRAW);
		}

		//Bind vertex data
		glBindBuffer(GL_ARRAY_BUFFER, _vertexDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, totalSprites * 4 * sizeof(TextureVertex2D), vertexData, GL_STATIC_DRAW);

		//Deallocate vertex data
		delete[] vertexData;
	}
	//No clips
	else
	{
		if (getTextureID() == 0)
		{
			printf("No texture to render with!\n");
		}

		if (_clip.size() <= 0)
		{
			printf("No clips to generate vertex data from!\n");
		}

		return false;
	}

	return true;
}

/******************************************************************************/
void SpriteSheet::freeSheet()
{
	if (_vertexDataBuffer != 0)
	{
		glDeleteBuffers(1, &_vertexDataBuffer);
		_vertexDataBuffer = 0;
	}

	if (_indexBuffers != nullptr)
	{
		glDeleteBuffers(_clip.size(), _indexBuffers);
		delete[] _indexBuffers;
		_indexBuffers = nullptr;
	}

	_clip.clear();
}

/******************************************************************************/
void SpriteSheet::freeTexture()
{
	freeSheet();

	Texture::freeTexture();
}

/******************************************************************************/
//TODO: After you have build the text program create the 2D program which will 
//be used to render the HUD
void SpriteSheet::renderSprite(int index)
{
	/**
	//Sprite sheet data exists
	if(vertexDataBuffer != NULL)
	{
		//Set texture
		glBindTexture(GL_TEXTURE_2D, getTextureID());

		//Enable vertex and texture coordinate arrays
		program2D.enableVertexPointer();
		program2D.enableTexCoordPointer();

		//Bind vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuffer);

		//Set texture coordinate data
		program2D.setTexCoordPointer(sizeof(TextureVertex2D), (GLvoid*)offsetof(TextureVertex2D, texCoord));

		//Set vertex data
		program2D.setVertexPointer(sizeof(TextureVertex2D), (GLvoid*)offsetof(TextureVertex2D, pos));

		//Draw quad using vertex data and index data
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffers[index]);
		glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

		//Disable vertex and texture coordinate arrays
		program2D.disableVertexPointer();
		program2D.disableTexCoordPointer();
	}
	*/
}
}; //wind