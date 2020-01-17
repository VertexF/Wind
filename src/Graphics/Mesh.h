#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

#include "../Physics/include/Polygon.h"
#include "OBJLoader.h"

class Vertex
{
	public:
		Vertex(){}
		Vertex(wind::Vector3 pos, const wind::Vector2& tex, const wind::Vector3& norm = wind::Vector3(0, 0, 0)) :
			pos(pos),
			tex(tex),
			norm(norm)
		{

		}

		void setPosition(wind::real x, wind::real y, wind::real z)
		{
			pos.x = x;
			pos.y = y;
			pos.z = z;
		}

		void setTexCoord(wind::real x, wind::real y)
		{
			tex.x = x;
			tex.y = y;
		}

		void setNormals(wind::real x, wind::real y, wind::real z)
		{
			norm.x = x;
			norm.y = y;
			norm.z = z;
			norm.normalise();
		}

		wind::Vector3 getPosition() const { return pos; }
		wind::Vector2 getTexCoord() const { return tex; }
		wind::Vector3 getNormals() const { return norm; }
	private:
		wind::Vector3 pos;
		wind::Vector2 tex;
		wind::Vector3 norm;
};

class Mesh
{
	public:
		Mesh(){}
		Mesh(Vertex* vertices, unsigned int numVertices, unsigned int *indices, unsigned numInderices);
		Mesh(const std::string& filePath, const wind::Vector3& size, std::vector<wind::Polygon*> &poly);
		Mesh(const std::string& filePath, const wind::Vector3& size);
		virtual ~Mesh();

		void addMesh(const std::string& filePath, const wind::Vector3& size, std::vector<wind::Polygon*> &poly);
		void addMesh(const std::string& filePath, const wind::Vector3& size);

		std::vector<wind::Vector3> getAllVertices();
		wind::Vector3 getFirstVertex();

		void Draw();
	private:

	    OBJModel model;
		void initMesh(const IndexedModel& model);

		enum
		{
			POSITION_VB,
			TEXCOORD_VB,
			NORMAL_VB,
			INDEX_VB,

			NUM_BUFFERS
		};

		GLuint vertexArrayObject;
		GLuint vertexArrayBuffers[NUM_BUFFERS];

		unsigned int drawCount;
};

#endif
