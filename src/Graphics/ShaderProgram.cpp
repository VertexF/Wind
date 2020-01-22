#include "ShaderProgram.h"

namespace wind
{

/******************************************************************************/
ShaderProgram::ShaderProgram() : _programID(0)
{
}

/******************************************************************************/
ShaderProgram::~ShaderProgram()
{
    freeProgram();
}
/******************************************************************************/
void ShaderProgram::freeProgram()
{
    glDeleteProgram(_programID);
}

/******************************************************************************/
void ShaderProgram::addVertexShader(const std::string& filepath)
{
    _shaders[VERTEX_SHADER] = loadShaderFromFile(filepath, GL_VERTEX_SHADER);
    glAttachShader(_programID, _shaders[VERTEX_SHADER]);

    if (_shaders[VERTEX_SHADER] == 0)
    {
        glDeleteProgram(_programID);
        _programID = 0;
    }
}

/******************************************************************************/
void ShaderProgram::addFragmentShader(const std::string& filepath)
{
    _shaders[FRAGMENT_SHADER] = loadShaderFromFile(filepath, GL_FRAGMENT_SHADER);
    glAttachShader(_programID, _shaders[FRAGMENT_SHADER]);

    //Check for errors
    if (_shaders[FRAGMENT_SHADER] == 0)
    {
        glDeleteShader(_shaders[VERTEX_SHADER]);
        glDeleteProgram(_programID);
        _programID = 0;
    }
}

/******************************************************************************/
void ShaderProgram::bind()
{
    glUseProgram(_programID);
}

/******************************************************************************/
void ShaderProgram::unbind()
{
    glUseProgram(0);
}

/******************************************************************************/
GLuint ShaderProgram::buildShaders(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
    _programID = glCreateProgram();

    addVertexShader(vertexFilePath);
    addFragmentShader(fragmentFilePath);

    return _programID;
}

/******************************************************************************/
GLuint ShaderProgram::getProgramID() const
{
    return _programID;
}

/******************************************************************************/
//These 2 functions have to use char raw pointers because that's how OpenGL works...
void ShaderProgram::printProgramLog(GLuint program) const
{
    //Make sure name is shader
    if (glIsProgram(program))
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
        {
            //Print Log
            std::cerr << infoLog << std::endl;
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        std::cerr << "Name " << program << " is not a program." << std::endl;
    }
}

/******************************************************************************/
void ShaderProgram::printShaderLog(GLuint shader) const
{
    //Make sure name is shader
    if (glIsShader(shader))
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
        {
            //Print Log
            std::cerr << infoLog << std::endl;
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        std::cerr << "Name " << shader << " is not a shader." << std::endl;
    }
}

void ShaderProgram::compileShaders()
{
    glLinkProgram(_programID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(_programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE)
    {
        std::cerr << "Error linking program: " << _programID << std::endl;
        printProgramLog(_programID);
        glDeleteShader(_shaders[VERTEX_SHADER]);
        glDeleteShader(_shaders[FRAGMENT_SHADER]);
        glDeleteProgram(_programID);
        _programID = 0;
    }

    glValidateProgram(_programID);

    glGetProgramiv(_programID, GL_VALIDATE_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE)
    {
        std::cerr << "Error validating program: " << _programID << std::endl;
        printProgramLog(_programID);
        glDeleteShader(_shaders[VERTEX_SHADER]);
        glDeleteShader(_shaders[FRAGMENT_SHADER]);
        glDeleteProgram(_programID);
        _programID = 0;
    }

    //Clean up excess shader references
    glDeleteShader(_shaders[VERTEX_SHADER]);
    glDeleteShader(_shaders[FRAGMENT_SHADER]);
}

GLuint ShaderProgram::loadShaderFromFile(const std::string &path, GLenum shaderType)
{
    //Open file
    GLuint shaderID = 0;
    std::string shaderString;
    std::ifstream sourceFile(path.c_str());

    //Source file loaded
    if (sourceFile)
    {
        //Get shader source
        shaderString.assign((std::istreambuf_iterator<char>(sourceFile)),
                             std::istreambuf_iterator<char>());

        //Create shader ID
        shaderID = glCreateShader(shaderType);

        //Set shader source
        const GLchar* shaderSourceChar[1];
        GLint shaderSourceLength[1];
        shaderSourceChar[0] = shaderString.c_str();
        shaderSourceLength[0] = shaderString.length();

        glShaderSource(shaderID, 1, shaderSourceChar, shaderSourceLength);

        //Compile shader source
        glCompileShader(shaderID);

        //Check shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
        if (shaderCompiled != GL_TRUE)
        {
            std::cerr << "Unable to compile shader " << shaderID << std::endl;
            std::cerr << "\n\nSource:\n\n" << shaderSourceChar[0] << std::endl;
            printShaderLog(shaderID);
            glDeleteShader(shaderID);
            shaderID = 0;
        }
    }
    else
    {
        std::cerr << "Unable to open file " << path.c_str() << std::endl;
    }

    return shaderID;
}
};