#include "Sublayer.h"
#include "RenderingLayer.h"

void RenderingLayer::Draw()
{
}

void RenderingLayer::DrawMesh(Mesh mesh)
{
	meshes.push_back(mesh);
}
void RenderingLayer::AddLight(Light light)
{
	lights.push_back(light);
}

void RenderingLayer::ResetMeshes()
{
	Mesh mesh = Mesh();
	meshes = { mesh };
	lights = { };
}
