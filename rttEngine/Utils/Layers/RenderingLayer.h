#pragma once

#include "Sublayer.h"
class Mesh;
#include "Objects\Mesh.h"
#include <vector>

class RenderingLayer: public Sublayer {
public: 
	RenderingLayer() = default;
	virtual void Draw();
	void DrawMesh(Mesh mesh);
	void ResetMeshes();
	std::vector<Mesh> meshes;
};