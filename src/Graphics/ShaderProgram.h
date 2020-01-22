#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>

#include "../Physics/include/Core.h"

namespace wind
{
class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram();

    virtual bool loadProgram() = 0;
    virtual void freeProgram();

    void bind();
    void unbind();

    GLuint getProgramID() const;

protected:
    enum
    {
        VERTEX_SHADER,
        FRAGMENT_SHADER,

        TOTAL_SHADERS
    };
protected:

    void printProgramLog(GLuint program) const;
    void printShaderLog(GLuint shader) const;

    GLuint loadShaderFromFile(const std::string &path, GLenum shaderType);

    void compileShaders();
    void addVertexShader(const std::string &filepath);
    void addFragmentShader(const std::string &filepath);
    GLuint buildShaders(const std::string &vertexFilePath, const std::string &fragmentFilePath);

    GLuint _programID;
    GLuint _shaders[TOTAL_SHADERS];
};
}; //wind
#endif
