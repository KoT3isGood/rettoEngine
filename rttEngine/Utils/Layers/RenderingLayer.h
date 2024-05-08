#pragma once

#include "Sublayer.h"
class Mesh;
class Light;
#include "Objects\Mesh.h"
#include <vector>

class RenderingLayer: public Sublayer {
public: 
	RenderingLayer() = default;
	virtual void Draw();
	void DrawMesh(Mesh mesh);
	void AddLight(Light light);
	void ResetMeshes();
	std::vector<Mesh> meshes;
	std::vector<Light> lights;
};