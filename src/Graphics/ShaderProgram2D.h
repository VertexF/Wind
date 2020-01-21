#ifndef RENDERING2D_H
#define RENDERING2D_H

#include <string>
#include <iostream>

#include "../Physics/include/Core.h"
#include "Texture.h"
#include "include/ColourRGBA.h"
#include "ShaderProgram.h"

namespace wind
{
/**
    In this class we are going to be using a texture, cutting it up and rendering
    it streght to the camera viewport using different shaders.
*/
class ShaderProgram2D : public ShaderProgram
{
public:
    ShaderProgram2D();

    void enableBlend();
    bool loadProgram();

    void setVertexPointer(GLsizei stride, const GLvoid* data);
    void setTexCoordPointer(GLsizei stride, const GLvoid* data);

    void enableVertexPointer();
    void disableVertexPointer();

    void enableTexCoordPointer();
    void disableTexCoordPointer();

    void setProjection(const Matrix4x4 &matrix);
    void setModelView(const Matrix4x4 &matrix);

    void leftMultProjection(const Matrix4x4 &matrix);
    void leftMultModelView(const Matrix4x4 &matrix);

    void updateProjection();
    void updateModelView();

    void setTextColor(const ColourRGBA &colour);
    void setTextureUnit(GLuint unit);
private:
    //Attribute locations
    GLint _vertexPos2DLocation;
    GLint _texCoordLocation;

    //Coloring location
    GLint _textColourLocation;

    //Texture unit location
    GLint _textureUnitLocation;

    //Projection matrix
    Matrix4x4 _projectionMatrix;
    GLint _projectionMatrixLocation;
    GLfloat _finalProjection[16];

    //Modelview matrix
    Matrix4x4 _modelViewMatrix;
    GLint _modelViewMatrixLocation;
    GLfloat _finalModel[16];
};
}; //wind
#endif
