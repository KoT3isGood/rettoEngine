#pragma once
#include <vector>
#include <string>
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\IO\InputOutput.h"
#include "Utils\Application\ProcessInfo.h"
#include "Utils\Layers\RenderingLayer.h"

struct rttGUIBase {
private:
	struct {
		bool leftMouseButton;
		bool rightMouseButton;
	} previnput, currentinput;
	std::string currentWindow = "";
	vec2 currentWindowPos;
	uint32_t currentWindowElement = 1;
public:
	uint32_t id = 0;
	std::vector<uint32_t> drawHierarchy;

	void NewFrame();
	vec4 Window(std::string name, vec4 windowsize);
	bool Button(std::string name);
	void Text(std::string name);
	void Render();
};

 rttGUIBase* rttGUI();