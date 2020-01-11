#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <fstream>
#include <glew.h>
#include <GL/glu.h>

#include "../Physics/include/Core.h"

namespace
{
	const static unsigned int TOTAL_SHADERS = 2;
}

class ShaderProgram
{
	public:
		ShaderProgram();
		virtual ~ShaderProgram();

		virtual bool loadProgram() = 0;
		virtual void freeProgram();

		void bind();
		void unbind();

		GLuint getProgramID();

	protected:
		std::string LoadShader(const std::string& fileName);

		void printProgramLog(GLuint program);
		void printShaderLog(GLuint shader);

		void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);

		GLuint loadShaderFromFile(std::string path, GLenum shaderType);
		GLuint CreateShader(const std::string& text, GLenum shadertype);

		void compileShaders();
		void addVertexShader(const std::string& filepath);
		void addFragmentShader(const std::string& filepath);
		GLuint buildShaders(const std::string& vertexFilePath, const std::string& fragmentFilePath);

		GLuint programID;
		GLuint shaders[TOTAL_SHADERS];
};

#endif
