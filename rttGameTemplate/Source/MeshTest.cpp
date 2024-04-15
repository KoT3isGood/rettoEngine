#include "MeshTest.h"

MeshTest::MeshTest()
{
	RTT_LOG("Inited");
	
}

void MeshTest::Tick(float deltaTime)
{

	mesh.Render();
}
