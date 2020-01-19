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

		void setProjection(wind::Matrix4x4 matrix);
		void setModelView(wind::Matrix4x4 matrix);

		void leftMultProjection(wind::Matrix4x4 matrix);
		void leftMultModelView(wind::Matrix4x4 matrix);

		void updateProjection();
		void updateModelView();

		void setTextColor(ColourRGBA colour);
		void setTextureUnit(GLuint unit);

		void updateModel(const std::vector<std::unique_ptr<wind::RigidBody>> &transforms, const std::vector<std::unique_ptr<Mesh>> &mesh);
		void updateCamera(Camera cam);

		void update(const std::vector<std::unique_ptr<wind::RigidBody>> &transforms, const std::vector<std::unique_ptr<Mesh>> &mesh, Camera cam);

        template<typename T>
		void drawModels(T mesh)
        {
            for(unsigned int i = 0; i < mesh.size(); i++)
            {
                //Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] every loop
                GLfloat tempModel[16] = {0};
                //Note: That this rotation is using RigidBody motion rather than any all transform matrix.
                mesh.at(i)->getBody()->getGLTransform(tempModel);

                //Then each matrix is passed into the shader but we need to add 1 to the starting position as not to conflict with the model view projection transform.
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, tempModel);

                mesh.at(i)->draw();
            }
        }

		template<typename T>
		void drawModel(T mesh)
		{
			//Each model needs it's own matrix model for translation that's why we recreate the GLfloat[] every loop
			GLfloat tempModel[16] = { 0 };
			//Note: That this rotation is using RigidBody motion rather than any all transform matrix.
			mesh->getBody()->getGLTransform(tempModel);

			//Then each matrix is passed into the shader but we need to add 1 to the starting position as not to conflict with the model view projection transform.
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, tempModel);

			mesh->draw();
		}


	private:
		//Attribute locations
		GLint vertexPos3DLocation;
		GLint indicesPos3DLocation;
		GLint texCoordLocation;

		//Coloring location
		GLint textColourLocation;

		//Texture unit location
		GLint textureUnitLocation;

		//Texture unit location
		GLint modelLocation;
		GLint cameraLocation;
		GLint normalLocation;

		//Projection matrix
		wind::Matrix4x4 projectionMatrix;
		GLint projectionMatrixLocation;
		GLfloat finalProjection[16];

		//Modelview matrix
		wind::Matrix4x4 modelViewMatrix;
		GLint modelViewMatrixLocation;
		GLfloat finalModel[16];
		GLfloat mfinal[16];
};

#endif
