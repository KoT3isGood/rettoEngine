#include "Sun.h"

Sun::Sun()
{
}

void Sun::Tick(float deltaTime)
{
	light.pos[0] = -3000;
	light.pos[1] = 2000;
	light.pos[2] = 10000;
	light.size = 500;

	light.color[0] = 20000;
	light.color[1] = 20000;
	light.color[2] = 20000;
	light.Render();
}
