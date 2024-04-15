#include "Mesh.h"


void Mesh::Render()
{
	getProcessInfo()->renderingLayer->DrawMesh(*this);
}
