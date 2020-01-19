#ifndef RENDERING2D_H
#define RENDERING2D_H

#include <string>
#include <iostream>

#include "../Physics/include/Core.h"
#include "Texture.h"
#include "include/ColourRGBA.h"
#include "ShaderProgram.h"

/**
	In this class we are going to be using a texture, cutting it up and rendering it streght to the camera viewport using different shaders.
*/
class ShaderProgram2D : public ShaderProgram
{
	public:
		ShaderProgram2D();

		void enableBlend();
		bool loadProgram();

		void setVertexPointer(GLsizei stride, const GLvoid* data);
		void setTexCoordPointer(GLsizei stride, const GLvoid* data);

		void enableVertexPointer();
		void disableVertexPointer();

		void enableTexCoordPointer();
		void disableTexCoordPointer();

		void setProjection(wind::Matrix4x4 matrix);
		void setModelView(wind::Matrix4x4 matrix);

		void leftMultProjection(wind::Matrix4x4 matrix);
		void leftMultModelView(wind::Matrix4x4 matrix);

		void updateProjection();
		void updateModelView();

		void setTextColor(ColourRGBA colour);
		void setTextureUnit(GLuint unit);
	private:
		//Attribute locations
		GLint vertexPos2DLocation;
		GLint texCoordLocation;

		//Coloring location
		GLint textColourLocation;

		//Texture unit location
		GLint textureUnitLocation;

		//Projection matrix
		wind::Matrix4x4 projectionMatrix;
		GLint projectionMatrixLocation;
		GLfloat finalProjection[16];

		//Modelview matrix
		wind::Matrix4x4 modelViewMatrix;
		GLint modelViewMatrixLocation;
		GLfloat finalModel[16];
};

#endif
