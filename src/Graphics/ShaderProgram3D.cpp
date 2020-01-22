#include "ShaderProgram3D.h"

namespace wind
{
/******************************************************************************/
ShaderProgram3D::ShaderProgram3D() : _vertexPos3DLocation(0), _indicesPos3DLocation(0),
_texCoordLocation(0), _textColourLocation(0), _textureUnitLocation(0),
_modelLocation(0), _cameraLocation(0), _normalLocation(0)
{
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
    glViewport(0.f, 0.f, 800, 600);
}

/******************************************************************************/
void ShaderProgram3D::disableBlend()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_BLEND);
}

/******************************************************************************/
bool ShaderProgram3D::loadProgram()
{
    buildShaders("res/basicShader.vsh", "res/basicShader.fsh");
    compileShaders();

    _vertexPos3DLocation = glGetAttribLocation(_programID, "vertexPosition3D");
    if (_vertexPos3DLocation == -1)
    {
        std::cerr << "vertexPosition3D is not a valid glsl program variable!" << std::endl;
    }

    _texCoordLocation = glGetAttribLocation(_programID, "TexCoord");
    if (_texCoordLocation == -1)
    {
        std::cerr << "TexCoord is not a valid glsl program variable!" << std::endl;
    }

    _normalLocation = glGetAttribLocation(_programID, "normal");
    if (_normalLocation == -1)
    {
        std::cerr << "normal is not a valid glsl program variable!" << std::endl;
    }

    _textColourLocation = glGetUniformLocation(_programID, "texColour");
    if (_textColourLocation == -1)
    {
        std::cerr << "textColour is not a valid glsl program variable!" << std::endl;
    }

    _textureUnitLocation = glGetUniformLocation(_programID, "textureUnit");
    if (_textureUnitLocation == -1)
    {
        std::cerr << "textureUnit is not a valid glsl program variable!" << std::endl;
    }

    _cameraLocation = glGetUniformLocation(_programID, "camera");
    if (_cameraLocation == -1)
    {
        std::cerr << "camera is not a valid glsl program variable!" << std::endl;
    }

    _modelLocation = glGetUniformLocation(_programID, "model");
    if (_modelLocation == -1)
    {
        std::cerr << "model is not a valid glsl program variable!" << std::endl;
    }

    return true;
}

/******************************************************************************/
void ShaderProgram3D::setVertexPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_vertexPos3DLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram3D::setIndicesPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_indicesPos3DLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram3D::setTexCoordPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_texCoordLocation, 2, GL_DOUBLE, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram3D::setNormalPointer(GLsizei stride, const GLvoid* data)
{
    glVertexAttribPointer(_normalLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

/******************************************************************************/
void ShaderProgram3D::enableVertexPointer()
{
    glEnableVertexAttribArray(_vertexPos3DLocation);
}

/******************************************************************************/
void ShaderProgram3D::disableVertexPointer()
{
    glDisableVertexAttribArray(_vertexPos3DLocation);
}

/******************************************************************************/
void ShaderProgram3D::enableIndicesPointer()
{
    glEnableVertexAttribArray(_indicesPos3DLocation);
}

/******************************************************************************/
void ShaderProgram3D::disableIndicesPointer()
{
    glDisableVertexAttribArray(_indicesPos3DLocation);
}

/******************************************************************************/
void ShaderProgram3D::enableTexCoordPointer()
{
    glEnableVertexAttribArray(_texCoordLocation);
}

/******************************************************************************/
void ShaderProgram3D::disableTexCoordPointer()
{
    glDisableVertexAttribArray(_texCoordLocation);
}

/******************************************************************************/
void ShaderProgram3D::enableNormalPointer()
{
    glEnableVertexAttribArray(_normalLocation);
}

/******************************************************************************/
void ShaderProgram3D::disableNormalPointer()
{
    glDisableVertexAttribArray(_normalLocation);
}

/******************************************************************************/
void ShaderProgram3D::setProjection(const Matrix4x4 &matrix)
{
    _projectionMatrix = matrix;
}

/******************************************************************************/
void ShaderProgram3D::setModelView(const Matrix4x4 &matrix)
{
    _modelViewMatrix = matrix;
}

/******************************************************************************/
void ShaderProgram3D::leftMultProjection(const Matrix4x4 &matrix)
{
    _projectionMatrix = _projectionMatrix * matrix;
}

/******************************************************************************/
void ShaderProgram3D::leftMultModelView(const Matrix4x4 &matrix)
{
    _modelViewMatrix = _modelViewMatrix * matrix;
}

/******************************************************************************/
void ShaderProgram3D::updateCamera(const Camera &cam)
{
    //Here we are using the Core.h maths to workout the perspective, rotation and translation through the camera instance.
    Matrix4x4 viewProjection = cam.getVP();
    //When then turn that matrix to a GLfloat matrix and pass it into the shader by a uniform.
    viewProjection.getGLTransform(_finalProjection);
    glUniformMatrix4fv(_cameraLocation, 1, GL_FALSE, _finalProjection);
}

/******************************************************************************/
void ShaderProgram3D::updateModel(const std::vector<std::unique_ptr<wind::RigidBody>>& transforms, 
                                  const std::vector<std::unique_ptr<Mesh>>& mesh)
{
    for (unsigned int i = 0; i < transforms.size(); i++)
    {
        //Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] 
        //every loop
        GLfloat tempModel[16] = { 0 };
        //Note: That this rotation is using RigidBody motion rather than any all transform matrix.
        transforms.at(i)->getGLTransform(tempModel);

        //Then each matrix is passed into the shader but we need to add 1 to the starting position as 
        //not to conflict with the model view projection transform.
        glUniformMatrix4fv(_modelLocation, 1, GL_FALSE, tempModel);

        //mesh.at(i)->Draw();
    }
}

/******************************************************************************/
void ShaderProgram3D::setTextColor(ColourRGBA colour)
{
    glUniform4f(_textColourLocation, colour.r, colour.g, colour.b, colour.a);
}

/******************************************************************************/
void ShaderProgram3D::setTextureUnit(GLuint unit)
{
    glUniform1i(_textureUnitLocation, unit);
}

/******************************************************************************/
void ShaderProgram3D::update(const std::vector<std::unique_ptr<wind::RigidBody>>& transforms,
                             const std::vector<std::unique_ptr<Mesh>>& mesh, const Camera &cam)
{
    updateCamera(cam);
    updateModel(transforms, mesh);
}
}; //wind