#include "Mesh.h"


void Mesh::Render()
{
	GetProcessInfo()->renderingLayer->DrawMesh(*this);
}

void Light::Render()
{
	GetProcessInfo()->renderingLayer->AddLight(*this);
}
