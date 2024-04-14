#pragma once
#include <vector>
#include <string>

class Actor {
public:
	Actor();
	virtual void Tick(float deltaTime);
	~Actor();

	uint64_t worldID = 0;
};