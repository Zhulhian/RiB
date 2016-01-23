#include <cstdio>
#include "libtcod.hpp"
#include "Actor.hpp"
#include "Map.hpp"
#include "Engine.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

Engine engine;

int main() {
	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	return 0;
}