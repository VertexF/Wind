#include "ShaderProgram2D.h"

textProgram::textProgram()
{
	vertexPos2DLocation = 0;
	texCoordLocation = 0;

	textColourLocation = 0;
	textureUnitLocation = 0;

	projectionMatrixLocation = 0;
	modelViewMatrixLocation = 0;

	//glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
	//glViewport(0.f, 0.f, 800, 600);
}

void textProgram::enableBlend()
{
	//Set blending
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool textProgram::loadProgram()
{
	programID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile("res/2DFragShader.vsh", GL_VERTEX_SHADER);

    //Check for errors
    if(vertexShader == 0)
    {
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

	//Attach vertex shader to program
	glAttachShader(programID, vertexShader);


	//Create fragment shader
	GLuint fragmentShader = loadShaderFromFile("res/2DFragShader.fsh", GL_FRAGMENT_SHADER);

    //Check for errors
    if(fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

	//Attach fragment shader to program
	glAttachShader(programID, fragmentShader);

	//Link program
    glLinkProgram(programID);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);
	if(programSuccess != GL_TRUE)
    {
		std::cerr << "Error linking program: " << programID << std::endl;
		printProgramLog(programID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

	//Clean up excess shader references
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	vertexPos2DLocation = glGetAttribLocation(programID, "vertexPosition2D");
	if(vertexPos2DLocation == -1)
	{
		std::cerr << "vertexPosition2D is not a valid glsl program variable!" << std::endl;
	}

	texCoordLocation = glGetAttribLocation(programID, "TexCoord");
	if(texCoordLocation == -1)
	{
		std::cerr << "TexCoord is not a valid glsl program variable!" << std::endl;
	}

	textColourLocation = glGetUniformLocation(programID, "texColour");
	if(textColourLocation == -1)
	{
		std::cerr << "textColor is not a valid glsl program variable!" << std::endl;
	}

	textureUnitLocation = glGetUniformLocation(programID, "textureUnit");
	if(textureUnitLocation == -1)
	{
		std::cerr << "textureUnit is not a valid glsl program variable!" << std::endl;
	}

	projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	if(projectionMatrixLocation == -1)
	{
		std::cerr << "projectionMatrix is not a valid glsl program variable!" << std::endl;
	}

	modelViewMatrixLocation = glGetUniformLocation(programID, "modelMatrix");
	if(modelViewMatrixLocation == -1)
	{
		std::cerr << "modelViewMatrix is not a valid glsl program variable!" << std::endl;
	}

	return true;
}

void textProgram::setVertexPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

void textProgram::setTexCoordPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

void textProgram::enableVertexPointer()
{
	glEnableVertexAttribArray(vertexPos2DLocation);
}

void textProgram::disableVertexPointer()
{
	glDisableVertexAttribArray(vertexPos2DLocation);
}

void textProgram::enableTexCoordPointer()
{
	glEnableVertexAttribArray(texCoordLocation);
}

void textProgram::disableTexCoordPointer()
{
	glDisableVertexAttribArray(texCoordLocation);
}

void textProgram::setProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = matrix;
}

void textProgram::setModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = matrix;
}

void textProgram::leftMultProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = projectionMatrix * matrix;
}

void textProgram::leftMultModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = modelViewMatrix * matrix;
}

void textProgram::updateProjection()
{
	projectionMatrix.getGLTransform(finalProjection);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, finalProjection);
}

void textProgram::updateModelView()
{
	modelViewMatrix.getGLTransform(finalModel);
	glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, finalModel);
}

void textProgram::setTextColor(ColourRGBA colour)
{
	glUniform4f(textColourLocation, colour.r, colour.g, colour.b, colour.a);
}

void textProgram::setTextureUnit(GLuint unit)
{
	glUniform1i(textureUnitLocation, unit);
}
