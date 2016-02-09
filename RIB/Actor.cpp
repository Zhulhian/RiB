#include "main.hpp"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &colFg, const TCODColor &colBg) :
	x(x), y(y), ch(ch), name(name), colFg(colFg), colBg(colBg), dead(false),
	blocks(true), attacker(NULL), destructible(NULL), ai(NULL),
	pickable(NULL), container(NULL) {
}

Actor::~Actor() {
	if (attacker) delete attacker;
	if (destructible) delete destructible;
	if (ai) delete ai;
	if (pickable) delete pickable;
	if (container) delete container;
}

void Actor::render() const {
	TCODColor colBg = dead ? TCODColor::lightRed : TCODConsole::root->getCharBackground(x, y);
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, colFg);
	TCODConsole::root->setCharBackground(x, y, colBg);
}

void Actor::update() {
	if (ai) ai->update(this);
}