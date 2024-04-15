#include "Level.h"

uint64_t worldIDCounter = 1;

void Level::InitActor(Actor* actor)
{
	actor->worldID = worldIDCounter;
	actors.push_back(actor);
	worldIDCounter += 1;
}

Actor* Level::GetActorByID(uint64_t id)
{
	if (id == 0) {
		return nullptr;
	}
	for (auto* actor : actors) {
		if (actor->worldID == id) {
			return actor;
		};
	};
	return nullptr;
}

void Level::Tick(float deltaTime)
{
	for (auto* actor : actors) {
		actor->Tick(deltaTime);
	};
}

Level* GetCurrentLevel()
{
	return getProcessInfo()->level;
}
