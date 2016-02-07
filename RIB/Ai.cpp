#include <stdio.h>
#include <math.h>
#include "main.hpp"

void PlayerAi::update(Actor *owner) {
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx = 0, dy = 0;

	switch (engine.lastKey.vk) {
	case TCODK_KP8:
	case TCODK_UP:			dy = -1;								break; // Up

	case TCODK_KP9:
	case TCODK_PAGEUP:		dx = 1;		dy = -1;					break; // North-east

	case TCODK_KP7:
	case TCODK_HOME:		dx = -1;	dy = -1;					break; // North-west

	case TCODK_KP1:
	case TCODK_PAGEDOWN:	dx = -1;	dy = 1;						break; // South-west

	case TCODK_KP3:
	case TCODK_END:			dx = 1;		dy = 1;						break; // South-east

	case TCODK_KP2:
	case TCODK_DOWN:		dy = 1;									break; // Down

	case TCODK_KP4:
	case TCODK_LEFT:		dx = -1;								break; // Left

	case TCODK_KP6:
	case TCODK_RIGHT:		dx = 1;									break; // Right

	case TCODK_5:
	case TCODK_KP5:
	case TCODK_SPACE:		engine.gameStatus = Engine::NEW_TURN;	break; // Wait

	case TCODK_CHAR:		handleActionKey(owner, engine.lastKey.c); break;
	default:				break;
	}
	if (dx != 0 || dy != 0) {
		engine.gameStatus = Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x + dx, owner->y + dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	// Returns true of move, false if attack ( and runs damage computations)
	
	if (engine.map->isWall(targetx, targety)) return false;

	// Look through actors and see if one is standing in spot you're about
	// to move to. If so, deal damage and return false.
	for (Actor **iterator = engine.actors.begin();
	iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->x == targetx && actor->y == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// Look through actors and see if there's any dead ones at
	// target location. If so, print it.
	for (Actor **iterator = engine.actors.begin();
	iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead())
			|| actor->pickable;
		if (corpseOrItem
			&& actor->x == targetx && actor->y == targety) {
			engine.gui->message(actor->col, "There's a %s lying here.", actor->name);
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = engine.player->container->size + 2;
	// INVENTORY WIDTH in width, then the size of the container + 2 (to get a frame around it)
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// Draw inventory frame
	con.setDefaultForeground(TCODColor::amber);
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true,
		TCOD_BKGND_DEFAULT, " I N V E N T O R Y ");

	// Display the items.
	con.setDefaultForeground(TCODColor::lighterGreen);
	int shortcut = 'a';
	int y = 1;
	for (Actor **it = owner->container->inventory.begin();
	it != owner->container->inventory.end(); it++) {
		Actor *actor = *it;
		TCODConsole::setColorControl(TCOD_COLCTRL_1, actor->col, TCODColor::black);
		con.print(2, y, "%c%c%c (%c) %s", TCOD_COLCTRL_1, actor->ch, TCOD_COLCTRL_STOP, shortcut, actor->name);
		y++;
		shortcut++;
	}
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2,
		engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
	TCODConsole::flush();
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) {
		int actorIndex = key.c - 'a';

		if (actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;	
}

void PlayerAi::showHelp() {
	static int HELP_WIDTH = 40;
	static int HELP_HEIGHT = engine.screenHeight - 30;

	static TCODConsole help(HELP_WIDTH, HELP_HEIGHT);

	help.setDefaultForeground(TCODColor::amber);
	help.printFrame(0, 0, HELP_WIDTH, HELP_HEIGHT, true, TCOD_BKGND_DEFAULT, " H E L P ");

	TCODList<char *> helpMessages;
	helpMessages.push(" M O V E M E N T ");
	helpMessages.push("");
	helpMessages.push("    7   8   9");
	helpMessages.push(""); 
	helpMessages.push("      \\ | / ");
	helpMessages.push("");
	helpMessages.push("    4 - 5 - 6");
	helpMessages.push("");
	helpMessages.push("      / | \\ ");
	helpMessages.push("");
	helpMessages.push("    1   2   3");
	helpMessages.push("");
	helpMessages.push("");
	helpMessages.push("5 / space : Wait a turn.");
	helpMessages.push("");
	helpMessages.push("i : show inventory.");
	helpMessages.push("");
	helpMessages.push("g : pick up an item from the ground.");
	helpMessages.push("");
	helpMessages.push("m : pause / unpause music.");
	helpMessages.push("");
	helpMessages.push("? : show this helpscreen.");
	helpMessages.push("");
	helpMessages.push("Press any key quit this helpscreen");

	help.setDefaultForeground(TCODColor::lighterGreen);
	int y = 2;

	for (char* msg : helpMessages) {
		help.print(2, y, msg);
		y++;
	}

	TCODConsole::blit(&help, 0, 0, HELP_WIDTH, HELP_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - HELP_WIDTH / 2,
		engine.screenHeight / 2 - HELP_HEIGHT / 2);
	TCODConsole::flush();
	
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	help.clear();
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch (ascii) {
	case 'g': // get item
	{
		bool found = false;
		for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
			Actor *actor = *iterator;
			if (actor->pickable && actor->x == owner->x && actor->y == owner->y) {
				if (actor->pickable->pick(actor, owner)) {
					found = true;
					engine.gui->message(TCODColor::lightAzure, "You pick up the %s.", actor->name);
					break;
				}
				else if (!found) {
					found = true;
					engine.gui->message(TCODColor::lightRed, "Your inventory is full!");
				}
			}
		}
		if (!found) {
			engine.gui->message(TCODColor::lighterGrey, "There's nothing to pick up here.");
		}
		engine.gameStatus = Engine::NEW_TURN;
	}
	break;
	case 'm':
	{
		Sound::togglePause();
	}
	break;
	case 'i':
	{
		Actor *actor = choseFromInventory(owner);
		if (actor) {
			actor->pickable->use(actor, owner);
			engine.gameStatus = Engine::NEW_TURN;
		}
	}
	break;
	case '?':
	{
		showHelp();
	}
	break;
	case '/':
		engine.player->destructible->hp = 999;
		engine.player->attacker->power = 999;
	}
}

static const int TRACKING_TURNS = 3;

void EnemyAi::update(Actor *owner) {
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	if (engine.map->isInFov(owner->x, owner->y)) {
		// Player sees the enemy.
		moveCount = TRACKING_TURNS;
	}
	else {
		moveCount--;
	}
	if (moveCount > 0) {
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void EnemyAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);

	float distance = sqrtf(dx * dx + dy * dy);

	if (distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));

		if (engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		}
		else if (engine.map->canWalk(owner->x + stepdx, owner->y)) {
			owner->x += stepdx;
		}
		else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
			owner->y += stepdy;
		}
	}
	else if (owner->attacker) {
			owner->attacker->attack(owner, engine.player);
	}
	
}