#include <stdio.h>
#include "main.hpp"

Attacker::Attacker(float power) : power(power) {
}

void Attacker::attack(Actor *owner, Actor *target) {
	TCODRandom *rng = TCODRandom::getInstance();
	float damage = rng->getInt(0, power) + rng->getInt(1, 3);
	if (target->destructible && !target->destructible->isDead()) {
		if (damage - target->destructible->defense > 0) {
			engine.gui->message(owner==engine.player ? TCODColor::crimson : TCODColor::azure, "%s attacks the %s for %g hit points.\n", owner->name, target->name, 
				damage - target->destructible->defense);
		}
		else {
			engine.gui->message(TCODColor::grey, "%s attacks the %s but fails to do any damage.\n", owner->name, target->name);
		}
		target->destructible->takeDamage(target, damage);
	}
	else {
		engine.gui->message(TCODColor::grey, "%s attacks the %s to no effect.\n", owner->name, target->name);
	}
}