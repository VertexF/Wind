#include "ShaderProgram.h"

ShaderProgram::ShaderProgram()
{
	programID = 0;
}

ShaderProgram::~ShaderProgram()
{
	freeProgram();
}

void ShaderProgram::freeProgram()
{
	glDeleteProgram(programID);
}

void ShaderProgram::addVertexShader(const std::string& filepath)
{
	shaders[VERTEX_SHADER] = loadShaderFromFile(filepath, GL_VERTEX_SHADER);
	glAttachShader(programID, shaders[VERTEX_SHADER]);

    if(shaders[VERTEX_SHADER] == 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}

void ShaderProgram::addFragmentShader(const std::string& filepath)
{
	shaders[FRAGMENT_SHADER] = loadShaderFromFile(filepath, GL_FRAGMENT_SHADER);
	glAttachShader(programID, shaders[FRAGMENT_SHADER]);

    //Check for errors
    if(shaders[FRAGMENT_SHADER] == 0)
    {
        glDeleteShader(shaders[VERTEX_SHADER]);
        glDeleteProgram(programID);
        programID = 0;
    }
}

void ShaderProgram::bind()
{
	glUseProgram(programID);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

GLuint ShaderProgram::buildShaders(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
	programID = glCreateProgram();

	addVertexShader(vertexFilePath);
	addFragmentShader(fragmentFilePath);

	return programID;
}

GLuint ShaderProgram::getProgramID()
{
	return programID;
}

//These 2 functions have to use char raw pointers because that's how OpenGL works...
void ShaderProgram::printProgramLog(GLuint program)
{
	//Make sure name is shader
	if(glIsProgram(program))
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
		if(infoLogLength > 0)
		{
			//Print Log
            std::cerr << infoLog << std::endl;
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		std::cerr << "Name " << program <<  " is not a program." << std::endl;
	}
}

void ShaderProgram::printShaderLog(GLuint shader)
{
	//Make sure name is shader
	if(glIsShader(shader))
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
		if(infoLogLength > 0)
		{
			//Print Log
			std::cerr << infoLog << std::endl;
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
	    std::cerr << "Name " << shader <<  " is not a shader." << std::endl;
	}
}

void ShaderProgram::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = {0};

    if(isProgram)
	{
        glGetProgramiv(shader, flag, &success);
	}
    else
	{
        glGetShaderiv(shader, flag, &success);
	}

    if(success == GL_FALSE)
    {
        if(isProgram)
		{
            glGetProgramInfoLog(shader, sizeof(error), nullptr, error);
		}
        else
		{
            glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
		}

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

void ShaderProgram::compileShaders()
{
	glLinkProgram(programID);
	CheckShaderError(programID, GL_LINK_STATUS, true, "Program failed to link: ");

	glValidateProgram(programID);
	CheckShaderError(programID, GL_VALIDATE_STATUS, true, "Program is invalid: ");
}

GLuint ShaderProgram::loadShaderFromFile(std::string path, GLenum shaderType)
{
	//Open file
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	//Source file loaded
	if(sourceFile)
	{
	    //Get shader source
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());

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
        if(shaderCompiled != GL_TRUE)
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
