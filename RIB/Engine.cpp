#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(40),
	screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::setCustomFont("terminal16x16_gs_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW | TCOD_FONT_TYPE_GRAYSCALE);
	TCODConsole::initRoot(screenWidth, screenHeight, "TelOps", false);

	
	player = new Actor(40, 25, '@', "Player", TCODColor::darkerGrey);
	player->destructible = new PlayerDestructible(30, 2, "Your corpse", '%');
	player->attacker = new Attacker(6);
	player->ai = new PlayerAi();
	player->container = new Container(10);

	actors.push(player);
	
	map = new Map(screenWidth - PANEL_WIDTH, screenHeight);
	gui = new Gui();
	gui->message(TCODColor::azure,
		"You have been deployed in the suburbs. Your goal is to eliminate the extraterrestial threat.");
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
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}