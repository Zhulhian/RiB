#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(40),
	screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::initRoot(screenWidth, screenHeight, "Rogues in Black", false);

	player = new Actor(40, 25, '@', "Player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "Your corpse");
	player->attacker = new Attacker(6);
	player->ai = new PlayerAi();

	actors.push(player);
	map = new Map(110, 80);
	gui = new Gui();

	gui->message(TCODColor::flame,
		"You open your eyes. You are in an \n building. Something feels wrong.");
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::update() {
	if (gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;

	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	player->update();

	if (gameStatus == NEW_TURN) {
		for (Actor **iterator = actors.begin(); iterator != actors.end();
		iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	
	// Draw the map:
	map->render();

	// Draw the actors:
	for (Actor **iterator = actors.begin(); iterator != actors.end();
		iterator++) {
		Actor *actor = *iterator;
		if (map->isInFov(actor->x, actor->y)) {
			actor->render();
		}
	}

	player->render();
	gui->render();

	TCODConsole::root->print(1, screenHeight - 2, "HP: %d/%d",
		(int)player->destructible->hp, (int)player->destructible->maxHp);
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}