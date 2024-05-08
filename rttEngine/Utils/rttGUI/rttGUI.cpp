#include "rttGUI.h"

rttGUIBase gui;

void rttGUIBase::NewFrame()
{
	currentinput.leftMouseButton = IsKeyDown(VK_LBUTTON);
	currentinput.rightMouseButton = IsKeyDown(VK_RBUTTON);
}

vec4 rttGUIBase::Window(std::string name, vec4 windowsize)
{
	currentWindowElement = 1;
	size_t size = drawHierarchy.size();
	uint32_t compressedTextSize = ceil(name.size() / 4.0f);
	drawHierarchy.resize(size + 1 + 1 + compressedTextSize + 4);
	drawHierarchy[size] = 1;
	drawHierarchy[size + 1] = name.size();
	for (uint32_t i = 0; i < compressedTextSize; i++) {
		drawHierarchy[size + 2 + i] = uint32_t(name[0]) << 24 + uint32_t(name[1]) << 16 + uint32_t(name[2]) << 8 + uint32_t(name[3]);
	}
	drawHierarchy[size + 2 + compressedTextSize] = windowsize.x;
	drawHierarchy[size + 3 + compressedTextSize] = windowsize.y;
	drawHierarchy[size + 4 + compressedTextSize] = windowsize.z;
	drawHierarchy[size + 5 + compressedTextSize] = windowsize.w;
	currentWindowPos = vec2(windowsize.x, windowsize.y);
	id++;
	return windowsize;
}

bool rttGUIBase::Button(std::string name)
{
	size_t size = drawHierarchy.size();
	uint32_t compressedTextSize = ceil(name.size()/4.0f);
	drawHierarchy.resize(size + 1 + 1 + compressedTextSize+4);
	drawHierarchy[size] = 2;
	drawHierarchy[size+1] = name.size();
	for (uint32_t i = 0; i < compressedTextSize; i++) {
		uint32_t result = 0;
			uint charactersLeft = name.size()-i*4;
			if (charactersLeft == 1) {
				result = (uint32_t(name[0 + i * 4]) << 24);	
			}
			else if(charactersLeft == 2) {
				result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16);
			}
			else if (charactersLeft == 3) {
				result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16) + (uint32_t(name[2 + i * 4]) << 8);
			}
			else {
				result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16) + (uint32_t(name[2 + i * 4]) << 8) + (uint32_t(name[3 + i * 4]));
			}
		drawHierarchy[size + 2 + i] = result;
	}
	vec2 minSize = vec2(8, 28*(currentWindowElement)-20)+currentWindowPos;
	vec2 maxSize = vec2(8+4+name.size()*16, 28 * (currentWindowElement))+currentWindowPos;
	drawHierarchy[size + 2 + compressedTextSize] = minSize.x;
	drawHierarchy[size + 3 + compressedTextSize] = minSize.y;
	drawHierarchy[size + 4 + compressedTextSize] = maxSize.x;
	drawHierarchy[size + 5 + compressedTextSize] = maxSize.y;
	id++;
	currentWindowElement++;

	vec2 mousePosX = vec2(GetMousePosition()[0], GetMousePosition()[1]);
	vec2 windowPos = vec2(getProcessInfo()->surface->positionY + 7, getProcessInfo()->surface->positionX+31);
	vec2 absPos = mousePosX - windowPos;


	if (absPos.x >= minSize.x && absPos.x <= maxSize.x && absPos.y >= minSize.y && absPos.y <= maxSize.y) {
		if (previnput.leftMouseButton&&!currentinput.leftMouseButton) {
			return true;
		};
	}

	return false;
}

void rttGUIBase::Text(std::string name)
{
	size_t size = drawHierarchy.size();
	uint32_t compressedTextSize = ceil(name.size() / 4.0f);
	drawHierarchy.resize(size + 1 + 1 + compressedTextSize + 4);
	drawHierarchy[size] = 3;
	drawHierarchy[size + 1] = name.size();
	for (uint32_t i = 0; i < compressedTextSize; i++) {
		uint32_t result = 0;
		uint charactersLeft = name.size() - i * 4;
		if (charactersLeft == 1) {
			result = (uint32_t(name[0 + i * 4]) << 24);
		}
		else if (charactersLeft == 2) {
			result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16);
		}
		else if (charactersLeft == 3) {
			result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16) + (uint32_t(name[2 + i * 4]) << 8);
		}
		else {
			result = (uint32_t(name[0 + i * 4]) << 24) + (uint32_t(name[1 + i * 4]) << 16) + (uint32_t(name[2 + i * 4]) << 8) + (uint32_t(name[3 + i * 4]));
		}
		drawHierarchy[size + 2 + i] = result;
	}
	vec2 minSize = vec2(8, 28 * (currentWindowElement)-20) + currentWindowPos;
	vec2 maxSize = vec2(8 + 4 + name.size() * 16, 28 * (currentWindowElement)) + currentWindowPos;
	drawHierarchy[size + 2 + compressedTextSize] = minSize.x;
	drawHierarchy[size + 3 + compressedTextSize] = minSize.y;
	drawHierarchy[size + 4 + compressedTextSize] = maxSize.x;
	drawHierarchy[size + 5 + compressedTextSize] = maxSize.y;
	id++;
	currentWindowElement++;
}

void rttGUIBase::Render()
{
	
	id = 0;
	drawHierarchy = {};
	previnput = currentinput;
}

rttGUIBase* rttGUI()
{
	return &gui;
}
