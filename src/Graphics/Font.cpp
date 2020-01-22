#include "Font.h"
#include "ShaderProgram2D.h"
#include "../Physics/include/Core.h"
#include "include/TextureVertex2D.h"
#include <iostream>

namespace wind
{

/******************************************************************************/
Font::Font() : _space(0),
    _lineHeight(0),
    _newLine(0)
{
}

/******************************************************************************/
Font::~Font()
{
    freeFont();
}

/******************************************************************************/
void Font::bind(unsigned int unit)
{
    assert(unit >= 0 && unit <= 31);

    glActiveTexture(GL_TEXTURE + unit);
    glBindTexture(GL_TEXTURE_2D, getTextureID());
}

/******************************************************************************/
bool Font::loadImage(const std::string &filePath)
{
    bool success = true;

    //So first we need to create a background pixel colour.
    const GLubyte BLACK_PIXEL = 0x00;

    freeFont();

    if (Texture::loadPixelsFromFile8(filePath))
    {
        GLfloat cellWidth = getImageWidth() / 16.0f;
        GLfloat cellHeight = getImageHeight() / 16.0f;

        //So these will hold the top and the bottom data of each letter.
        GLuint top = cellHeight;
        GLuint bottom = 0;
        GLuint aBottom = 0;

        //These hold the pixel coordinates for the letter.
        int pX = 0;
        int pY = 0;

        //These hold base offset values.
        int bX = 0;
        int bY = 0;

        //Here we start cutting up the image
        GLuint currentChar = 0;
        FontRect nextClip = { 0, 0, cellWidth, cellHeight };
        for (unsigned int rows = 0; rows < 16; rows++)
        {
            for (unsigned int cols = 0; cols < 16; cols++)
            {
                //First we need to set off the offset.
                bX = cellWidth * cols;
                bY = cellHeight * rows;

                nextClip.x = cellWidth * cols;
                nextClip.y = cellHeight * rows;

                nextClip.w = cellWidth;
                nextClip.h = cellHeight;

                //Now each loops goes through and finds the top, bottom, left and right of the text.
                //Find the left side
                for (int Lcols = 0; Lcols < cellWidth; Lcols++)
                {
                    for (int Lrows = 0; Lrows < cellHeight; Lrows++)
                    {
                        pX = bX + Lcols;
                        pY = bY + Lrows;

                        if (getPixel8(pX, pY) != BLACK_PIXEL)
                        {
                            nextClip.x = pX;

                            Lrows = cellWidth;
                            Lcols = cellHeight;
                        }
                    }
                }

                //Find the right side
                for (int Rcols = cellWidth - 1; Rcols >= 0; Rcols--)
                {
                    for (int Rrows = 0; Rrows < cellHeight; Rrows++)
                    {
                        pX = bX + Rcols;
                        pY = bY + Rrows;

                        if (getPixel8(pX, pY) != BLACK_PIXEL)
                        {
                            nextClip.w = (pX - nextClip.x) + 1;

                            Rcols = -1;
                            Rrows = cellHeight;
                        }
                    }
                }

                //Find the top side
                for (int Trows = 0; Trows < cellHeight; Trows++)
                {
                    for (int Tcols = 0; Tcols < cellWidth; Tcols++)
                    {
                        pX = bX + Tcols;
                        pY = bY + Trows;

                        if (getPixel8(pX, pY) != BLACK_PIXEL)
                        {
                            if (Trows < top)
                            {
                                top = Trows;
                            }

                            Tcols = cellWidth;
                            Trows = cellHeight;
                        }
                    }
                }

                //Find the bottom side
                for (int Brows = cellHeight - 1; Brows >= 0; Brows--)
                {
                    for (int Bcols = 0; Bcols < cellWidth; Bcols++)
                    {
                        pX = bX + Bcols;
                        pY = bY + Brows;

                        if (getPixel8(pX, pY) != BLACK_PIXEL)
                        {
                            if (currentChar == 'A')
                            {
                                aBottom = Brows;
                            }

                            if (Brows > bottom)
                            {
                                bottom = Brows;
                            }

                            Bcols = cellWidth;
                            Brows = -1;
                        }
                    }
                }

                _clip.push_back(nextClip);
                currentChar++;
            }
        }

        for (int t = 0; t < 256; t++)
        {
            _clip[t].y += top;
            _clip[t].h -= top;
        }

        if (loadTextureFromPixels8())
        {
            if (!generateDataBuffer(SPRITE_ORIGIN_TOP_LEFT))
            {
                std::cerr << "Unable to create vertex buffer for bitmap font!" << std::endl;
                success = false;
            }
        }
        else
        {
            std::cerr << "Unable to create texture from bitmap font pixels!" << std::endl;
            success = false;
        }

        glBindTexture(GL_TEXTURE_2D, getTextureID());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        _space = cellWidth / 2;
        _newLine = aBottom - top;
        _lineHeight = bottom - top;
    }
    else
    {
        std::cerr << "Could not load bitmap from file: " << filePath << std::endl;
        success = false;
    }

    return success;
}

/******************************************************************************/
void Font::freeFont()
{
    freeTexture();

    _space = 0;
    _lineHeight = 0;
    _newLine = 0;
}

void Font::renderText(ShaderProgram2D *fontProgram2D, GLfloat x, GLfloat y, 
                      const std::string &text, FontRect *area, int align)
{
    //If there is a texture to render from
    if (getTextureID() != 0)
    {
        //Draw positions
        GLfloat dX = x;
        GLfloat dY = y;

        //If the text needs to be aligned
        if (area != nullptr)
        {
            //Correct empty alignment
            if (align == 0)
            {
                align = FONT_TEXT_ALIGN_LEFT | FONT_TEXT_ALIGN_TOP;
            }

            //Handle horizontal alignment
            if (align & FONT_TEXT_ALIGN_LEFT)
            {
                dX = area->x;
            }
            else if (align & FONT_TEXT_ALIGN_CENTERED_H)
            {
                dX = area->x + (area->w - substringWidth(text.c_str())) / 2.f;
            }
            else if (align & FONT_TEXT_ALIGN_RIGHT)
            {
                dX = area->x + (area->w - substringWidth(text.c_str()));
            }

            //Handle vertical alignment
            if (align & FONT_TEXT_ALIGN_TOP)
            {
                dY = area->y;
            }
            else if (align & FONT_TEXT_ALIGN_CENTERED_V)
            {
                dY = area->y + (area->h - stringHeight(text.c_str())) / 2.f;
            }
            else if (align & FONT_TEXT_ALIGN_BOTTOM)
            {
                dY = area->y + (area->h - stringHeight(text.c_str()));
            }
        }

        //This resets the font for drawing.
        fontProgram2D->setModelView(wind::Matrix4x4());

        //Move to draw position
        wind::Matrix4x4 MV;
        MV.setTranslation(dX, dY, 0.f);
        fontProgram2D->leftMultModelView(MV);

        //Set texture
        glBindTexture(GL_TEXTURE_2D, getTextureID());

        //Enable vertex and texture coordinate arrays
        fontProgram2D->enableVertexPointer();
        fontProgram2D->enableTexCoordPointer();

        //Bind vertex data
        glBindBuffer(GL_ARRAY_BUFFER, _vertexDataBuffer);

        //Set vertex data
        fontProgram2D->setVertexPointer(sizeof(TextureVertex2D), (GLvoid*)offsetof(TextureVertex2D, pos));

        //Set texture coordinate data
        fontProgram2D->setTexCoordPointer(sizeof(TextureVertex2D), (GLvoid*)offsetof(TextureVertex2D, texCoord));

        //Go through string
        for (int i = 0; i < text.length(); i++)
        {
            //Space
            if (text[i] == ' ')
            {
                wind::Matrix4x4 MV;
                MV.setTranslation(_space, 0.f, 0.f);
                fontProgram2D->leftMultModelView(MV);
                fontProgram2D->updateModelView();

                dX += _space;
            }
            //Newline
            else if (text[i] == '\n')
            {
                //Handle horizontal alignment
                GLfloat targetX = x;
                if (area != NULL)
                {
                    if (align & FONT_TEXT_ALIGN_LEFT)
                    {
                        targetX = area->x;
                    }
                    else if (align & FONT_TEXT_ALIGN_CENTERED_H)
                    {
                        targetX = area->x + (area->w - substringWidth(&text.c_str()[i + 1])) / 2.f;
                    }
                    else if (align & FONT_TEXT_ALIGN_RIGHT)
                    {
                        targetX = area->x + (area->w - substringWidth(&text.c_str()[i + 1]));
                    }
                }

                //Move to target point
                wind::Matrix4x4 MV;
                MV.setTranslation(targetX - dX, _newLine, 0.f);
                fontProgram2D->leftMultModelView(MV);
                fontProgram2D->updateModelView();

                dY += _newLine;
                dX += targetX - dX;
            }
            //Character
            else
            {
                //Get ASCII
                GLuint ascii = static_cast<unsigned char>(text[i]);

                //Update position matrix in program
                fontProgram2D->updateModelView();

                //Draw quad using vertex data and index data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[ascii]);
                glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, nullptr);

                wind::Matrix4x4 MV;
                MV.setTranslation(_clip[ascii].w, 0.f, 0.f);
                fontProgram2D->leftMultModelView(MV);
                fontProgram2D->updateModelView();
                dX += _clip[ascii].w;
            }
        }

        //Disable vertex and texture coordinate arrays
        fontProgram2D->disableVertexPointer();
        fontProgram2D->disableTexCoordPointer();

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

/******************************************************************************/
GLfloat Font::getLineHeight() const
{
    return _lineHeight;
}

/******************************************************************************/
FontRect Font::getAreaString(const std::string& text) const
{
    //Initialize area
    GLfloat subWidth = 0.f;
    FontRect area = { 0.f, 0.f, subWidth, _lineHeight };

    //Go through string
    for (int i = 0; i < text.length(); i++)
    {
        //Space
        if (text[i] == ' ')
        {
            subWidth += _space;
        }
        //Newline
        else if (text[i] == '\n')
        {
            //Add another line
            area.h += _lineHeight;

            //Check for max width
            if (subWidth > area.w)
            {
                area.w = subWidth;
                subWidth = 0.f;
            }
        }
        //Character
        else
        {
            //Get ASCII
            GLuint ascii = static_cast<unsigned char>(text[i]);
            subWidth += _clip[ascii].w;
        }
    }

    //Check for max width
    if (subWidth > area.w)
    {
        area.w = subWidth;
    }

    return area;
}

/******************************************************************************/
GLfloat Font::substringWidth(const char *subtext) const
{
    GLfloat subWidth = 0.f;

    //Go through string
    for (int i = 0; i < subtext[i] != '\0' && subtext[i] != '\n'; i++)
    {
        //Space
        if (subtext[i] == ' ')
        {
            subWidth += _space;
        }
        //Character
        else
        {
            //Get ASCII
            GLuint ascii = static_cast<unsigned char>(subtext[i]);
            subWidth += _clip[ascii].w;
        }
    }

    return subWidth;
}

/******************************************************************************/
GLfloat Font::stringHeight(const char *text) const
{
    GLfloat height = _lineHeight;

    //Go through string
    for (int i = 0; text[i] != '\0'; i++)
    {
        //Space
        if (text[i] == '\n')
        {
            height += _lineHeight;
        }
    }
    return height;
}
}; //wind