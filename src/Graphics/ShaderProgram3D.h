#ifndef SHADERPROGRAM3D_H
#define SHADERPROGRAM3D_H

#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include "Texture.h"
#include "include/ColourRGBA.h"
#include "ShaderProgram.h"
#include "../Camera.h"
#include "Mesh.h"

namespace wind
{
class ShaderProgram3D : public ShaderProgram
{
public:
    ShaderProgram3D();

    void disableBlend();
    bool loadProgram();

    void setVertexPointer(GLsizei stride, const GLvoid* data);
    void setIndicesPointer(GLsizei stride, const GLvoid* data);
    void setTexCoordPointer(GLsizei stride, const GLvoid* data);
    void setNormalPointer(GLsizei stride, const GLvoid* data);

    void enableVertexPointer();
    void disableVertexPointer();

    void enableIndicesPointer();
    void disableIndicesPointer();

    void enableTexCoordPointer();
    void disableTexCoordPointer();

    void enableNormalPointer();
    void disableNormalPointer();

    void setProjection(const Matrix4x4 &matrix);
    void setModelView(const Matrix4x4 &matrix);

    void leftMultProjection(const Matrix4x4 &matrix);
    void leftMultModelView(const Matrix4x4 &matrix);

    void setTextColor(ColourRGBA colour);
    void setTextureUnit(GLuint unit);

    void updateModel(const std::vector<std::unique_ptr<wind::RigidBody>>& transforms, 
                     const std::vector<std::unique_ptr<Mesh>>& mesh);
    void updateCamera(const Camera &cam);

    void update(const std::vector<std::unique_ptr<wind::RigidBody>>& transforms, 
                const std::vector<std::unique_ptr<Mesh>>& mesh, const Camera &cam);

    //This template model takes in the a vector of the models of the same type and renders them.
    template<typename T>
    void drawModels(T &mesh)
    {
        for (unsigned int i = 0; i < mesh.size(); i++)
        {
            //Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] every loop
            GLfloat tempModel[16] = { 0 };
            //Note: That this rotation is using RigidBody motion rather than any all transform matrix.
            mesh.at(i)->getBody()->getGLTransform(tempModel);

            //Then each matrix is passed into the shader but we need to add 1 to the starting position as not to conflict with the model view projection transform.
            glUniformMatrix4fv(_modelLocation, 1, GL_FALSE, tempModel);

            mesh.at(i)->draw();
        }
    }

    //This template model takes in the a single models of the same type and renders them.
    template<typename T>
    void drawModel(T &mesh)
    {
        //Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] every loop
        GLfloat tempModel[16] = { 0 };
        //Note: That this rotation is using RigidBody motion rather than any all transform matrix.
        mesh->getBody()->getGLTransform(tempModel);

        //Then each matrix is passed into the shader but we need to add 1 to the starting position as not to conflict with the model view projection transform.
        glUniformMatrix4fv(_modelLocation, 1, GL_FALSE, tempModel);

        mesh->draw();
    }

public:
    //Attribute locations
    GLint _vertexPos3DLocation;
    GLint _indicesPos3DLocation;
    GLint _texCoordLocation;

    //Coloring location
    GLint _textColourLocation;

    //Texture unit location
    GLint _textureUnitLocation;

    //Texture unit location
    GLint _modelLocation;
    GLint _cameraLocation;
    GLint _normalLocation;

    //Projection matrix
    Matrix4x4 _projectionMatrix;
    GLfloat _finalProjection[16];

    //Modelview matrix
    Matrix4x4 _modelViewMatrix;
};
}; //wind
#endif
