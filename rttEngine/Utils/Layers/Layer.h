#pragma once
#include "Sublayer.h"
#include <vector>

class Layer {
public:
	// Default constructor
	Layer() = default;

	// Used to push back sublayer into layer
	/// ~~Intellisense
	/// <summary>Used to push back sublayer into layer</summary>
	/// <param name="layer">Layer to pushback</param>
	void PushBack(Sublayer* layer);

	// Used to pop back lastest sublayer in layer
	/// ~~Intellisense
	/// <summary>Used to pop back lastest sublayer in layer</summary>
	void PopBack();


	// Used to get all children in layer
	/// ~~Intellisense
	/// <summary>Used to get all children in layer</summary>
	/// <returns>Returns children components</returns>
	std::vector<Sublayer*> GetChildren();

private:

	// Children sublayers
	std::vector<Sublayer*> children;
};
