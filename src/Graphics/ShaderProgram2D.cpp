#include "ShaderProgram2D.h"

namespace wind
{

/******************************************************************************/
ShaderProgram2D::ShaderProgram2D() : _vertexPos2DLocation(0), _texCoordLocation(0),
_textColourLocation(0), _textureUnitLocation(0), _projectionMatrixLocation(0),
_modelViewMatrixLocation(0)
{
}

/******************************************************************************/
void ShaderProgram2D::enableBlend()
{
    //Set blending
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/******************************************************************************/
bool ShaderProgram2D::loadProgram()
{
    buildShaders("res/2DFragShader.vsh", "res/2DFragShader.fsh");
    compileShaders();

    _vertexPos2DLocation = glGetAttribLocation(_programID, "vertexPosition2D");
    if (_vertexPos2DLocation == -1)
    {
        std::cerr << "vertexPosition2D is not a valid glsl program variable!" << std::endl;
    }

    _texCoordLocation = glGetAttribLocation(_programID, "TexCoord");
    if (_texCoordLocation == -1)
    {
        std::cerr << "TexCoord is not a valid glsl program variable!" << std::endl;
    }

    _textColourLocation = glGetUniformLocation(_programID, "texColour");
    if (_textColourLocation == -1)
    {
        std::cerr << "textColor is not a valid glsl program variable!" << std::endl;
    }

    _textureUnitLocation = glGetUniformLocation(_programID, "textureUnit");
    if (_textureUnitLocation == -1)
    {
        std::cerr << "textureUnit is not a valid glsl program variable!" << std::endl;
    }

    _projectionMatrixLocation = glGetUniformLocation(_programID, "projectionMatrix");
    if (_projectionMatrixLocation == -1)
    {
        std::cerr << "projectionMatrix is not a valid glsl program variable!" << std::endl;
    }

    _modelViewMatrixLocation = glGetUniformLocation(_programID, "modelMatrix");
    if (_modelViewMatrixLocation == -1)
    {
        std::cerr << "modelViewMatrix is not a valid glsl program variable!" << std::endl;
    }

    return true;
}

/******************************************************************************/
void ShaderProgram2D::setVertexPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram2D::setTexCoordPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_texCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram2D::enableVertexPointer()
{
    glEnableVertexAttribArray(_vertexPos2DLocation);
}

/******************************************************************************/
void ShaderProgram2D::disableVertexPointer()
{
    glDisableVertexAttribArray(_vertexPos2DLocation);
}

/******************************************************************************/
void ShaderProgram2D::enableTexCoordPointer()
{
    glEnableVertexAttribArray(_texCoordLocation);
}

/******************************************************************************/
void ShaderProgram2D::disableTexCoordPointer()
{
    glDisableVertexAttribArray(_texCoordLocation);
}

/******************************************************************************/
void ShaderProgram2D::setProjection(const Matrix4x4 &matrix)
{
    _projectionMatrix = matrix;
}

/******************************************************************************/
void ShaderProgram2D::setModelView(const Matrix4x4 &matrix)
{
    _modelViewMatrix = matrix;
}

/******************************************************************************/
void ShaderProgram2D::leftMultProjection(const Matrix4x4 &matrix)
{
    _projectionMatrix = _projectionMatrix * matrix;
}

/******************************************************************************/
void ShaderProgram2D::leftMultModelView(const Matrix4x4 &matrix)
{
    _modelViewMatrix = _modelViewMatrix * matrix;
}

/******************************************************************************/
void ShaderProgram2D::updateProjection()
{
    _projectionMatrix.getGLTransform(_finalProjection);
    glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_FALSE, _finalProjection);
}

/******************************************************************************/
void ShaderProgram2D::updateModelView()
{
    _modelViewMatrix.getGLTransform(_finalModel);
    glUniformMatrix4fv(_modelViewMatrixLocation, 1, GL_FALSE, _finalModel);
}

/******************************************************************************/
void ShaderProgram2D::setTextColor(const ColourRGBA &colour)
{
    glUniform4f(_textColourLocation, colour.r, colour.g, colour.b, colour.a);
}

/******************************************************************************/
void ShaderProgram2D::setTextureUnit(GLuint unit)
{
    glUniform1i(_textureUnitLocation, unit);
}
}; //wind