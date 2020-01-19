#include "ShaderProgram2D.h"

ShaderProgram2D::ShaderProgram2D()
{
	vertexPos2DLocation = 0;
	texCoordLocation = 0;

	textColourLocation = 0;
	textureUnitLocation = 0;

	projectionMatrixLocation = 0;
	modelViewMatrixLocation = 0;
}

void ShaderProgram2D::enableBlend()
{
	//Set blending
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool ShaderProgram2D::loadProgram()
{
    buildShaders("res/2DFragShader.vsh", "res/2DFragShader.fsh");
    compileShaders();

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);
	if(programSuccess != GL_TRUE)
    {
		std::cerr << "Error linking program: " << programID << std::endl;
		printProgramLog(programID);
        glDeleteShader(shaders[VERTEX_SHADER]);
        glDeleteShader(shaders[FRAGMENT_SHADER]);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

	//Clean up excess shader references
    glDeleteShader(shaders[VERTEX_SHADER]);
    glDeleteShader(shaders[FRAGMENT_SHADER]);

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

void ShaderProgram2D::setVertexPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

void ShaderProgram2D::setTexCoordPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data);
}

void ShaderProgram2D::enableVertexPointer()
{
	glEnableVertexAttribArray(vertexPos2DLocation);
}

void ShaderProgram2D::disableVertexPointer()
{
	glDisableVertexAttribArray(vertexPos2DLocation);
}

void ShaderProgram2D::enableTexCoordPointer()
{
	glEnableVertexAttribArray(texCoordLocation);
}

void ShaderProgram2D::disableTexCoordPointer()
{
	glDisableVertexAttribArray(texCoordLocation);
}

void ShaderProgram2D::setProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = matrix;
}

void ShaderProgram2D::setModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = matrix;
}

void ShaderProgram2D::leftMultProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = projectionMatrix * matrix;
}

void ShaderProgram2D::leftMultModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = modelViewMatrix * matrix;
}

void ShaderProgram2D::updateProjection()
{
	projectionMatrix.getGLTransform(finalProjection);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, finalProjection);
}

void ShaderProgram2D::updateModelView()
{
	modelViewMatrix.getGLTransform(finalModel);
	glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, finalModel);
}

void ShaderProgram2D::setTextColor(ColourRGBA colour)
{
	glUniform4f(textColourLocation, colour.r, colour.g, colour.b, colour.a);
}

void ShaderProgram2D::setTextureUnit(GLuint unit)
{
	glUniform1i(textureUnitLocation, unit);
}
