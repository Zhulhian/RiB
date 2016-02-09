#include <stdarg.h>
#include <stdio.h>
#include <string>
#include "main.hpp"

static const int BAR_WIDTH = PANEL_WIDTH - 4;
static const int BAR_X = (PANEL_WIDTH - BAR_WIDTH) / 2;
static const int BAR_Y = 3;

//static const int MSG_X = (PANEL_WIDTH - MSG_WIDTH) / 2 + 1;
static const int MSG_Y = BAR_Y + 2;

// We subtract by 3 because we want one tile of free space at each side
// of the message, and we subtract one more because of the border, which
// takes up one tile.
static const char MSG_WIDTH = PANEL_WIDTH - 4;


Gui::Gui() {
	con = new TCODConsole(PANEL_WIDTH, engine.screenHeight);
}

Gui::~Gui() {
	delete con;
	log.clearAndDelete();
}

void Gui::render() {
	// Clear GUI console
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	
	// Render the border
	//renderBorder('#', TCODColor::grey);
	con->setDefaultForeground(TCODColor::azure);
	con->printFrame(0, 0, PANEL_WIDTH, engine.screenHeight, true, TCOD_BKGND_DEFAULT, "S T A T U S");
	
	// Render the health bar to the GUI console.
	renderBar(BAR_X, BAR_Y, BAR_WIDTH, "HP", engine.player->destructible->hp,
		engine.player->destructible->maxHp,
		TCODColor::lighterRed, TCODColor::lightRed);

	// Render the message log
	int y = MSG_Y;
	float colorCoef = 0.8f;

	for (Message **it = log.begin(); it != log.end(); it++) {
		Message *message = *it;

		con->setDefaultForeground(message->col * colorCoef);
		con->printRect(2, y, MSG_WIDTH, getMessageHeight(), message->text);
		y += con->getHeightRect(2, y, MSG_WIDTH, getMessageHeight(), message->text);
		if (colorCoef < 1.0f) {
			colorCoef += 0.2f;
		}
	}

	renderMouseLook();

	// Blit the GUI console on the root console
	TCODConsole::blit(con, 0, 0, PANEL_WIDTH, engine.screenHeight,
		TCODConsole::root, engine.screenWidth - PANEL_WIDTH, 0);
}

//void Gui::renderBorder(int symbol, const TCODColor &borderColor) {
//	con->setDefaultForeground(borderColor);
//	for (int y = 0; y <= engine.screenHeight; y++) {
//		con->setChar(0, y, symbol);
//		con->setCharForeground(0, y, borderColor);
//	}
//}

int Gui::getMessageHeight() {
	return (int)(engine.screenHeight / 3);
}

void Gui::renderBar(int x, int y, int width, const char *name,
	float value, float maxValue, const TCODColor &barColor,
	const TCODColor &backColor) {

	con->setDefaultBackground(backColor);
	con->rect(x, y, width, 1, false, TCOD_BKGND_SET);

	// How much of the bar should be filled with color?
	int barWidth = (int)(value / maxValue * width);

	if (barWidth > 0) {
		// Draw the bar
		con->setDefaultBackground(barColor);
		con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	// Print text on top of bar
	con->setDefaultForeground(TCODColor::red);
	con->setDefaultBackground(TCODColor::lighterRed);
	con->printEx((x + width / 2) , y - 1, TCOD_BKGND_SET, TCOD_CENTER,
		"+ %g/%g +", value, maxValue);
}

Gui::Message::Message(const char *text, const TCODColor &col) :
	text(strdup(text)), col(col) {
}

Gui::Message::~Message() {
	free(text);
}

void Gui::message(const TCODColor &col, const char *text, ...) {
	// Build the text using filthy black magic C functions.
	// Man page http://linux.die.net/man/3/va_end for more info

	// Make a variadic, basically a type to handle
	// a varying number of arguments of varying types.
	va_list ap;

	// Make a character buffer (array of characters - a string),
	// 128 bytes in size.
	// Warning for buffer overflow~!
	char buf[256];

	// Three functions for stepping through the variadict and formatting
	// correctly according to format chars such as %s, etc.
	va_start(ap, text);
	vsnprintf(buf, 256, text, ap);
	va_end(ap);

	int log_height = 0;

	for (Message **it = log.begin(); it != log.end(); it++) {
		Message *message = *it;
		log_height += con->getHeightRect(2, engine.screenHeight - getMessageHeight(), MSG_WIDTH, getMessageHeight(), message->text);
	}

	if (log_height >= getMessageHeight()) {
		Message *toRemove = log.get(0);
		log.remove(toRemove);
		delete toRemove;
	}

	Message *msg = new Message(buf, col);

	log.push(msg);
}

void Gui::renderMouseLook() {
	if (!engine.map->isInFov(engine.mouse.cx, engine.mouse.cy)) {
		return;
	}

	// Look into using string instead. Less performant
	// but simpler.
	char buf[128] = "";

	bool first = true;

	TCODColor mouseTargetColor;

	int descriptx = engine.mouse.cx;
	int descripty = engine.mouse.cy;

	for (Actor **it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor *actor = *it;

		if (actor->x == engine.mouse.cx && actor->y == engine.mouse.cy) {
			if (!first) {
				strcat(buf, ", ");
			}
			else {
				first = false;
			}
			strcat(buf, actor->name);
			mouseTargetColor = actor->colFg;
		}
	}
	
	TCODConsole::root->setDefaultBackground(TCODColor::black);
	TCODConsole::root->setDefaultForeground(mouseTargetColor);
	TCODConsole::root->setBackgroundFlag(TCOD_BKGND_SET);

	if (descriptx + strlen(buf) > engine.map->width) {
		descriptx = engine.map->width - strlen(buf);
	}

	TCODConsole::root->print(descriptx, engine.mouse.cy + 2, buf);
}