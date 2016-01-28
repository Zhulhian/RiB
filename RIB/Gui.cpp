#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include "main.hpp"

static const int PANEL_WIDTH = 15;

static const int BAR_WIDTH = 12;
static const int BAR_X = 1;
static const int BAR_Y = 1;

static const int MSG_HEIGHT = (int)(round(engine.screenHeight / 4));


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
	
	// Render the health bar to the GUI console.
	renderBar(BAR_X, BAR_Y, BAR_WIDTH, "HP", engine.player->destructible->hp,
		engine.player->destructible->maxHp,
		TCODColor::lighterRed, TCODColor::lightRed);

	// Render the message log
	int y = 3;
	float colorCoef = 0.4f;
	for (Message **it = log.begin(); it != log.end(); it++) {
		Message *message = *it;
		con->setDefaultForeground(message->col * colorCoef);
		con->print(BAR_X, y, message->text);
		y++;
		if (colorCoef < 1.0f) {
			colorCoef += 0.1f;
		}
	}

	renderMouseLook();

	// Blit the GUI console on the root console
	TCODConsole::blit(con, 0, 0, PANEL_WIDTH, engine.screenHeight,
		TCODConsole::root, engine.screenWidth - PANEL_WIDTH, 0);
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
	con->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER,
		"%s: %g/%g", name, value, maxValue);
}

Gui::Message::Message(const char *text, const TCODColor &col) :
	col(col) {
	this->text = new char[strlen(text)];
	strcpy(this->text,text);
}

Gui::Message::~Message() {
	delete [] text;
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
	char buf[128];

	// Three functions for stepping through the variadict
	va_start(ap, text);
	vsprintf(buf, text, ap);
	va_end(ap);

	// Line wrapping!
	char *lineBegin = buf;
	char *lineEnd;

	do {

		if (log.size() == MSG_HEIGHT) {
			Message *toRemove = log.get(0);
			log.remove(toRemove);
			delete toRemove;
		}
		
		lineEnd = strchr(lineBegin, '\n');
		if (lineEnd) {
			*lineEnd = '\0';
		}
		
		Message *msg = new Message(lineBegin, col);
		log.push(msg);

		lineBegin = lineEnd + 1;
	} while (lineEnd);
}

void Gui::renderMouseLook() {
	if (!engine.map->isInFov(engine.mouse.cx, engine.mouse.cy)) {
		return;
	}

	// Look into using string instead. Less performant
	// but simpler.
	char buf[128] = "";

	bool first = true;

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
		}
	}
	con->setDefaultForeground(TCODColor::lightGreen);
	con->print(1, 0, buf);
}