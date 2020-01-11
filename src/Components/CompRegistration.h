#ifndef COMPREGISTRATION_H
#define COMPREGISTRATION_H

#include <memory>
#include "../Graphics/Mesh.h"

class MeshReg
{
	protected:
	public:
		~MeshReg();
		void add(Mesh *m);
		void remove(unsigned int unit = 0);
		void draw();
		std::vector<std::unique_ptr<Mesh>> Regy;
};

#endif