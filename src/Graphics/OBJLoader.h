#ifndef OBJ_LOADER_H_INCLUDED
#define OBJ_LOADER_H_INCLUDED

#include <vector>
#include <string>
#include "../Physics/include/Core.h"

struct OBJIndex
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;

    bool operator<(const OBJIndex& r) const { return vertexIndex < r.vertexIndex; }
};

class IndexedModel
{
	public:
		std::vector<wind::Vector3> positions;
		std::vector<wind::Vector2> texCoords;
		std::vector<wind::Vector3> normals;
		std::vector<unsigned int> indices;

		void CalcNormals();
};

class OBJModel
{
	public:
		std::vector<OBJIndex> OBJIndices;
		std::vector<wind::Vector3> vertices;
		std::vector<wind::Vector2> uvs;
		std::vector<wind::Vector3> normals;
		std::vector<wind::Vector3> face;
		bool hasUVs;
		bool hasNormals;

		OBJModel(){}
		OBJModel(const std::string& fileName, const bool isStatic);
		void buildModel(const std::string& fileName, const bool isStatic);

		IndexedModel ToIndexedModel(const wind::Vector3& size);
	private:
		unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result);
		void CreateOBJFace(const std::string& line, const bool isStatic);

		wind::Vector2 ParseOBJVec2(const std::string& line);
		wind::Vector3 ParseOBJVec3(const std::string& line);
		OBJIndex ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals);
};

#endif
