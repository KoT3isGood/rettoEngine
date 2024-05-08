#include "Mesh.h"


void Mesh::Render()
{
	getProcessInfo()->renderingLayer->DrawMesh(*this);
}

void Light::Render()
{
	getProcessInfo()->renderingLayer->AddLight(*this);
}
