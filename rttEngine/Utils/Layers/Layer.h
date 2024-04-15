#pragma once
#include "Sublayer.h"
#include <vector>

class Layer {
public:
	// Default constructor
	Layer() = default;

	// Used to push back sublayer into layer
	void PushBack(Sublayer* layer);

	// Used to pop back lastest sublayer in layer
	void PopBack();


	// Used to get all children in layer
	std::vector<Sublayer*> GetChildren();

private:

	// Children sublayers
	std::vector<Sublayer*> children;
};
