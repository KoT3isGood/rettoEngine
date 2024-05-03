#include "MeshTest.h"


MeshTest::MeshTest()
{
	RTT_LOG("Inited");
	
}

void MeshTest::Tick(float deltaTime)
{
	time += deltaTime;
	rttGUI()->Window("Profiler", vec4(0, 0, 200, 200));
	rttGUI()->Text("frame time: " + std::to_string(int(deltaTime*100000)/100.0)+"ms");
	getProcessInfo()->camera[0][3] = -10.0;
	getProcessInfo()->camera[1][3] = 0.0;
	getProcessInfo()->camera[2][3] = 2.0;

	rttGUI()->Text("Camera position: " + std::to_string(getProcessInfo()->camera[0][3])+", "+ std::to_string(getProcessInfo()->camera[1][3]) +", " + std::to_string(getProcessInfo()->camera[2][3]));
	if (rttGUI()->Button("Toggle Drawing")) {
		shouldDraw = shouldDraw ? 0 : 1;
	};

	mesh.instanceID = 1;
	if (shouldDraw) {
		mesh.Render();
	}
}
