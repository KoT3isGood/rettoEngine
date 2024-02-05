#include "Sublayer.h"

Sublayer::Sublayer()
{
}

Sublayer::~Sublayer()
{
}

Layer* Sublayer::GetParent()
{
	return parent;
}

void Sublayer::SetParent(Layer* layer)
{
	if (!layer) {
		RTT_LOG("Parent is nullptr");
	}
	parent = layer;
}
