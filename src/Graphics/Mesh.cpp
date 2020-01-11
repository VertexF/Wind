#include "Mesh.h"

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int *indices, unsigned numInderices)
{
	IndexedModel model;

	for(unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(vertices[i].getPosition());
		model.texCoords.push_back(vertices[i].getTexCoord());
		model.normals.push_back(vertices[i].getNormals());
	}

	for(unsigned int i = 0; i < numInderices; i++)
	{
		model.indices.push_back(indices[i]);
	}

	initMesh(model);
}

Mesh::Mesh(const std::string& filePath, const wind::Vector3& size)
{
    model.buildModel(filePath, 0);
	IndexedModel iModel = model.ToIndexedModel(size);
	initMesh(iModel);
}

Mesh::Mesh(const std::string& filePath, const wind::Vector3& size, std::vector<wind::Polygon*> &poly)
{
    int getter = 0;
    OBJModel jack(filePath, 1);
    for(int i = 0; i < jack.face.size(); i++)
    {
        poly.push_back(new wind::Polygon());

        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
    }

	IndexedModel model = jack.ToIndexedModel(size);
	initMesh(model);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayObject);
}

void Mesh::addMesh(const std::string& filePath, const wind::Vector3& size, std::vector<wind::Polygon*> &poly)
{
    int getter = 0;
    OBJModel jack(filePath, 1);
    for(int i = 0; i < jack.face.size(); i++)
    {
        poly.push_back(new wind::Polygon());

        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
        poly[i]->vertices.push_back(jack.face[getter]);
        getter++;
    }

	IndexedModel model = jack.ToIndexedModel(size);
	initMesh(model);
}

void Mesh::addMesh(const std::string& filePath, const wind::Vector3& size)
{
    model.buildModel(filePath, 0);
	IndexedModel iModel = model.ToIndexedModel(size);
	initMesh(iModel);
}

std::vector<wind::Vector3> Mesh::getAllVertices()
{
    if(model.vertices.empty())
    {
        std::cerr << "No vertices dude, you ran this before you built the model" << std::endl;
        assert(!model.vertices.empty());
    }

    return model.vertices;
}

wind::Vector3 Mesh::getFirstVertex()
{
    if(model.vertices.empty())
    {
        std::cerr << "No vertices dude, you ran this before you built the model" << std::endl;
        assert(!model.vertices.empty());
    }

    return model.vertices[0];
}

void Mesh::initMesh(const IndexedModel& model)
{
	drawCount = model.indices.size();

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
    glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(vertexArrayObject);

	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
