#ifndef FONT_H
#define FONT_H

#include "SpriteSheet.h"
#include "include/FontRect.h"

namespace
{
enum FontTextAlignment
{
    FONT_TEXT_ALIGN_LEFT = 1,
    FONT_TEXT_ALIGN_CENTERED_H = 2,
    FONT_TEXT_ALIGN_RIGHT = 4,
    FONT_TEXT_ALIGN_TOP = 8,
    FONT_TEXT_ALIGN_CENTERED_V = 16,
    FONT_TEXT_ALIGN_BOTTOM = 32
};
}; //Anon

namespace wind
{
/*
    The primary function of this class is to load and cut up both TTF files and bitmap files into approate textures to be rendered.
**/

class ShaderProgram2D;

class Font : public SpriteSheet
{
public:
    Font();
    ~Font();
    //This function is going to set up the text program so we can use it for what we need.
    //Note: It's a pointer arguement.
    virtual void bind(unsigned int unit) override;

    //This function will use an image file, cut it up ready for rendering.
    bool loadImage(const std::string &filePath);

    //This function free the font up
    void freeFont();

    //This is the function that is going to render the text
    void renderText(ShaderProgram2D *fontProgram2D, GLfloat x, GLfloat y, 
                    const std::string &text, FontRect *area, int align = FONT_TEXT_ALIGN_LEFT);

    GLfloat getLineHeight() const;
    FontRect getAreaString(const std::string &text) const;
    GLfloat substringWidth(const char* subtext) const;
    GLfloat stringHeight(const char* text) const;
private:
    //These are the spacing varaibles.
    GLfloat _space;
    GLfloat _lineHeight;
    GLfloat _newLine;
};
}; //wind
#endif
