#include "main.hpp"

Spritesheet::Spritesheet(const char* filename) {
	TCODImage *spritesheet = new TCODImage(filename);
}

Spritesheet::~Spritesheet() {
}