#pragma once
#include "Actor.h"
#include "Utils\Application\ProcessInfo.h"

class Level {
public:
	Level() = default;

	// Adds actor to the level
	void InitActor(Actor* actor);

	// Returns actor by id
	Actor* GetActorByID(uint64_t id);

	// Should be accesed only from ProcessInfo in order to update actors every frame
	void Tick(float deltaTime);
private:
	// TODO: Replace with unordered map
	// List of all actors
	std::vector<Actor*> actors;
};

// Returns Current Level Pointer
Level* GetCurrentLevel();