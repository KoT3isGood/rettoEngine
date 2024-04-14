#pragma once
#include "Actor.h"

class Level {
public:
	Level() = default;

	void InitActor(Actor* actor);

	Actor* GetActorByID(uint64_t id);
	// DO NOT USE!!! Should be accesed only from ProcessInfo in order to update actors every frame
	void Tick(float deltaTime);
private:
	std::vector<Actor*> actors;
};