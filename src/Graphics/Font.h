#ifndef FONT_H
#define FONT_H

#include <iostream>

#include "../Physics/include/Core.h"
#include "ShaderProgram2D.h"
#include "SpriteSheet.h"
#include "include/FontRect.h"
#include "include/TextureVertex2D.h"

enum FontTextAlignment
{
    FONT_TEXT_ALIGN_LEFT = 1,
    FONT_TEXT_ALIGN_CENTERED_H = 2,
    FONT_TEXT_ALIGN_RIGHT = 4,
    FONT_TEXT_ALIGN_TOP = 8,
    FONT_TEXT_ALIGN_CENTERED_V = 16,
    FONT_TEXT_ALIGN_BOTTOM = 32
};

/*
	The primary function of this class is to load and cut up both TTF files and bitmap files into approate textures to be rendered.
**/
class Font : public SpriteSheet
{
	public:
		Font();
		~Font();
		//This function is going to set up the text program so we can use it for what we need.
		//Note: It's a pointer arguement.
		virtual void bind(unsigned int unit) final;

		//This function is going to set up the FreeType library used for every Font object.
		static bool initFreeType();

		//This function will use an image file, cut it up ready for rendering.
		bool loadImage(std::string filePath);

		//This function will use an TTF file, cut it up ready for rendering.
		//bool loadFreeType(std::string filePath, GLuint pixelSize);

		//This function free the font up
		void freeFont();

		//This is the function that is going to render the text
		void renderText(textProgram* fontProgram2D, GLfloat x, GLfloat y, std::string text, FontRect* area, int align = FONT_TEXT_ALIGN_LEFT);

		GLfloat getLineHeight();
		FontRect getAreaString(const std::string& text);
		GLfloat substringWidth(const char* subtext);
		GLfloat stringHeight(const char* text);
	private:
		//These are the spacing varaibles.
        GLfloat space;
        GLfloat lineHeight;
        GLfloat newLine;
};

#endif
