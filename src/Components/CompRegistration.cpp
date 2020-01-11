#include "CompRegistration.h"

MeshReg::~MeshReg()
{
	for(unsigned int i = 0; i < Regy.size(); i++)
	{
		Regy[i].release();
	}
}

void MeshReg::add(Mesh *m)
{
	Regy.push_back(std::move(std::unique_ptr<Mesh>(m)));
}

void MeshReg::remove(unsigned int unit)
{
	//Regy.erase(Regy.begin() + unit);
}

void MeshReg::draw()
{
	for(unsigned int i = 0; i < Regy.size(); i++)
	{
		Regy.at(i)->Draw();
	}
}