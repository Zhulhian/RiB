#include "main.hpp"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
const int PANEL_WIDTH = 37;
Engine engine(123, 67);

int main() {
	Sound::initialize();
	Sound::load("data/music/Sci_Fi_Industries_-_10_-_Jyib.mp3");
	Sound::play();

	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	return 0;
}