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
	shaders[0] = CreateShader(LoadShader(filepath), GL_VERTEX_SHADER);
	glAttachShader(programID, shaders[0]);
}

void ShaderProgram::addFragmentShader(const std::string& filepath)
{
	shaders[1] = CreateShader(LoadShader(filepath), GL_FRAGMENT_SHADER);
	glAttachShader(programID, shaders[1]);
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
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a program\n", program);
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
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a shader\n", shader);
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
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		}
        else
		{
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

std::string ShaderProgram::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

GLuint ShaderProgram::CreateShader(const std::string& text, GLenum shadertype)
{
	GLuint shader = glCreateShader(shadertype);

	if(shader == 0)
	{
		std::cerr << "Shader could not be created " << std::endl;
	}

	const GLchar* shaderSourceChar[1];
	GLint shaderSourceLength[1];
	shaderSourceChar[0] = text.c_str();
	shaderSourceLength[0] = text.length();

	glShaderSource(shader, 1, shaderSourceChar, shaderSourceLength);

	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Shader has failed to compile ");

	return shader;
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
	std::ifstream sourceFile( path.c_str() );

	//Source file loaded
	if(sourceFile)
	{
	    //Get shader source
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());

		//Create shader ID
		shaderID = glCreateShader(shaderType);

        //Set shader source
        const GLchar* shaderSource = shaderString.c_str();
        glShaderSource( shaderID, 1, (const GLchar**)&shaderSource, NULL );

        //Compile shader source
        glCompileShader(shaderID);

        //Check shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
        if( shaderCompiled != GL_TRUE )
        {
            printf("Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
            printShaderLog(shaderID);
            glDeleteShader(shaderID);
            shaderID = 0;
        }
	}
    else
    {
        printf("Unable to open file %s\n", path.c_str());
    }

	return shaderID;
}
