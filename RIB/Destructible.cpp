#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, const char *corpseName, int corpseTile) :
	maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName), corpseTile(corpseTile) {
}

EnemyDestructible::EnemyDestructible(float maxHp, float defense, const char *corpseName, int corpseTile) :
	Destructible(maxHp, defense, corpseName, corpseTile) {
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName, int corpseTile) :
	Destructible(maxHp, defense, corpseName, corpseTile) {
}

ObjectDestructible::ObjectDestructible(float maxHp, float defense, const char *brokenName, int brokenTile ) :
	Destructible(maxHp, defense, brokenName, brokenTile) {
}


float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if (damage > 0) {
		hp -= damage;
		if (hp <= 0) {
			hp = 0;
			die(owner);
		}
	}
	else {
		damage = 0;
	}
	return damage;
}

float Destructible::heal(float amount) {
	hp += amount;
	if (hp > maxHp) {
		amount -= hp - maxHp;
		hp = maxHp;
	}
	return amount;
}

void Destructible::die(Actor *owner) {
	// Transform the actor into a corpse! Yay!
	owner->ch = corpseTile;
	owner->name = corpseName;
	owner->blocks = false;

	// Make sure the corpses are drawn behind living actors
	engine.sendToBack(owner);
}

void EnemyDestructible::die(Actor *owner) {
	// Transform it into a corpse!
	engine.gui->message(TCODColor::lightestGrey, "+ The %s is dead. +", owner->name);
	owner->colFg = TCODColor::white;
	owner->dead = true;
	Destructible::die(owner);
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red, "+ You have met your end. +");
	Destructible::die(owner);
	owner->colFg = TCODColor::white;
	owner->dead = true;
	engine.gameStatus = Engine::DEFEAT;
}

void ObjectDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::sepia, "The %s breaks.", owner->name);
	Destructible::die(owner);
}