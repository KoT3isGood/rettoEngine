#include "StaticMesh.h"

StaticMesh::StaticMesh()
{
	position = vec3(0, 0, 0);
}

void StaticMesh::Tick(float deltaTime)
{
	Mesh mesh = Mesh();
	mesh.instanceID = 2;
	mesh.pos[0] = position.x;
	mesh.pos[1] = position.y;
	mesh.pos[2] = position.z;
	mesh.Render();
}
