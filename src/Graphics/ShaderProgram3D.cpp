#include "ShaderProgram3D.h"

ShaderProgram3D::ShaderProgram3D()
{
	vertexPos3DLocation = 0;
	indicesPos3DLocation = 0;
	texCoordLocation = 0;

	textColourLocation = 0;
	textureUnitLocation = 0;

	modelLocation = 0;
	cameraLocation = 0;

	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
	glViewport(0.f, 0.f, 800, 600);
}


void ShaderProgram3D::disableBlend()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_BLEND);
}

bool ShaderProgram3D::loadProgram()
{
    buildShaders("res/basicShader.vsh", "res/basicShader.fsh");
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

    vertexPos3DLocation = glGetAttribLocation(programID, "vertexPosition3D");
	if(vertexPos3DLocation == -1)
	{
		std::cerr << "vertexPosition3D is not a valid glsl program variable!" << std::endl;
	}

	texCoordLocation = glGetAttribLocation(programID, "TexCoord");
	if(texCoordLocation == -1)
	{
		std::cerr << "TexCoord is not a valid glsl program variable!" << std::endl;
	}

	normalLocation = glGetAttribLocation(programID, "normal");
	if(normalLocation == -1)
	{
		std::cerr << "normal is not a valid glsl program variable!" << std::endl;
	}

    textColourLocation = glGetUniformLocation(programID, "texColour");
	if(textColourLocation == -1)
	{
		std::cerr << "textColour is not a valid glsl program variable!" << std::endl;
	}

	textureUnitLocation = glGetUniformLocation(programID, "textureUnit");
	if(textureUnitLocation == -1)
	{
		std::cerr << "textureUnit is not a valid glsl program variable!" << std::endl;
	}

	cameraLocation = glGetUniformLocation(programID, "camera");
	if(cameraLocation == -1)
	{
		std::cerr << "camera is not a valid glsl program variable!" << std::endl;
	}

	modelLocation = glGetUniformLocation(programID, "model");
	if(modelLocation == -1)
	{
		std::cerr << "model is not a valid glsl program variable!" << std::endl;
	}

	return true;
}

void ShaderProgram3D::setVertexPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(vertexPos3DLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

void ShaderProgram3D::setIndicesPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(indicesPos3DLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

void ShaderProgram3D::setTexCoordPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(texCoordLocation, 2, GL_DOUBLE, GL_FALSE, stride, data);
}

void ShaderProgram3D::setNormalPointer(GLsizei stride, const GLvoid* data)
{
	glVertexAttribPointer(normalLocation, 4, GL_DOUBLE, GL_FALSE, stride, data);
}

void ShaderProgram3D::enableVertexPointer()
{
	glEnableVertexAttribArray(vertexPos3DLocation);
}

void ShaderProgram3D::disableVertexPointer()
{
	glDisableVertexAttribArray(vertexPos3DLocation);
}

void ShaderProgram3D::enableIndicesPointer()
{
	glEnableVertexAttribArray(indicesPos3DLocation);
}

void ShaderProgram3D::disableIndicesPointer()
{
	glDisableVertexAttribArray(indicesPos3DLocation);
}

void ShaderProgram3D::enableTexCoordPointer()
{
	glEnableVertexAttribArray(texCoordLocation);
}

void ShaderProgram3D::disableTexCoordPointer()
{
	glDisableVertexAttribArray(texCoordLocation);
}

void ShaderProgram3D::enableNormalPointer()
{
	glEnableVertexAttribArray(normalLocation);
}

void ShaderProgram3D::disableNormalPointer()
{
	glDisableVertexAttribArray(normalLocation);
}

void ShaderProgram3D::setProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = matrix;
}

void ShaderProgram3D::setModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = matrix;
}

void ShaderProgram3D::leftMultProjection(wind::Matrix4x4 matrix)
{
	projectionMatrix = projectionMatrix * matrix;
}

void ShaderProgram3D::leftMultModelView(wind::Matrix4x4 matrix)
{
	modelViewMatrix = modelViewMatrix * matrix;
}

void ShaderProgram3D::updateCamera(Camera cam)
{
	//Here we are using the Core.h maths to workout the perspective, rotation and translation through the camera instance.
	wind::Matrix4x4 viewProjection = cam.getVP();
	//When then turn that matrix to a GLfloat matrix and pass it into the shader by a uniform.
	viewProjection.getGLTransform(finalProjection);
	glUniformMatrix4fv(cameraLocation, 1, GL_FALSE, finalProjection);
}

void ShaderProgram3D::updateModel(const std::vector<std::unique_ptr<wind::RigidBody>> &transforms, const std::vector<std::unique_ptr<Mesh>> &mesh)
{
	for(unsigned int i = 0; i < transforms.size(); i++)
	{
		//Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] every loop
		GLfloat tempModel[16] = {0};
		//Note: That this rotation is using RigidBody motion rather than any all transform matrix.
		transforms.at(i)->getGLTransform(tempModel);

		//Then each matrix is passed into the shader but we need to add 1 to the starting position as not to conflict with the model view projection transform.
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, tempModel);

		//mesh.at(i)->Draw();
	}
}

void ShaderProgram3D::setTextColor(ColourRGBA colour)
{
	glUniform4f(textColourLocation, colour.r, colour.g, colour.b, colour.a);
}

void ShaderProgram3D::setTextureUnit(GLuint unit)
{
	glUniform1i(textureUnitLocation, unit);
}

void ShaderProgram3D::update(const std::vector<std::unique_ptr<wind::RigidBody>> &transforms, const std::vector<std::unique_ptr<Mesh>> &mesh, Camera cam)
{
	updateCamera(cam);
	updateModel(transforms, mesh);
}
