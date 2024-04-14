#include "Layer.h"

void Layer::PushBack(Sublayer *layer)
{
	size_t size = children.size();
	children.resize(children.size()+1);
	children[size] = layer;
}

void Layer::PopBack()
{
	children.pop_back();
}

std::vector<Sublayer*> Layer::GetChildren()
{
	return children;
}