#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <vector>

#include "Texture.h"
#include "ShaderProgram2D.h"
#include "include/TextureVertex2D.h"
#include "include/FontRect.h"


/*
    This enum holds different start positions to start drawing from
**/
enum SpriteOrigin
{
    SPRITE_ORIGIN_CENTER,
    SPRITE_ORIGIN_TOP_LEFT,
    SPRITE_ORIGIN_BOTTOM_LEFT,
    SPRITE_ORIGIN_TOP_RIGHT,
    SPRITE_ORIGIN_BOTTOM_RIGHT
};


namespace wind
{
/*
    This class is going to handle spritesheeting textures.
**/
class SpriteSheet : public Texture
{
public:
    SpriteSheet();
    ~SpriteSheet();

    virtual void bind(unsigned int unit);

    //This function adds a clip sprite to clip vector and returns index of clipping rectangle within clip array.
    int addClipSprite(const FontRect& newClip);

    //This function based of what index is passed in returns that clip rect.
    FontRect getRect(int index) const;

    //This is the main loading function while also creating a VBO and IBO.
    bool generateDataBuffer(SpriteOrigin orgin = SPRITE_ORIGIN_CENTER);

    //This function cleans up texture data
    void freeSheet();

    //Frees the texture and the sheet
    void freeTexture();

    //This functions renders the texture spredsheet
    void renderSprite(int index);
protected:
    //This will hold the font rect for the sprite in a sprite sheet.
    std::vector<FontRect> _clip;

    //VBO data
    GLuint _vertexDataBuffer;
    GLuint* _indexBuffers;
};
}; //wind
#endif
