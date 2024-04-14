#include "Level.h"


void Level::InitActor(Actor* actor)
{
	actors.push_back(actor);
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
