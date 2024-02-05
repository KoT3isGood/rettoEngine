#pragma once

#include "Runtime\Logging\Logger.h"
class Layer;


class Sublayer {
public: 
	Sublayer();
	~Sublayer();
	Layer* GetParent();
	void SetParent(Layer* layer);
private:
	Layer* parent = nullptr;
};