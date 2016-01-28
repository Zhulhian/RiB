#include <stdio.h>
#include <math.h>
#include "main.hpp"

void PlayerAi::update(Actor *owner) {
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx = 0, dy = 0;

	switch (engine.lastKey.vk) {
	case TCODK_UP:			dy = -1;				break;
	case TCODK_PAGEUP:		dx = 1;		dy = -1;	break;
	case TCODK_HOME:		dx = -1;	dy = -1;	break;
	case TCODK_PAGEDOWN:	dx = -1;	dy = 1;		break;
	case TCODK_END:			dx = 1;		dy = 1;		break;
	case TCODK_DOWN:		dy = 1;					break;
	case TCODK_LEFT:		dx = -1;				break;
	case TCODK_RIGHT:		dx = 1;					break;
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
		if (actor->destructible && actor->destructible->isDead()
			&& actor->x == targetx && actor->y == targety) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s lying here.\n", actor->name);
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;



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