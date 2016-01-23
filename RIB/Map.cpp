#include "libtcod.hpp"
#include "Map.hpp"

static const int HOUSE_MAX_SIZE = 12;
static const int HOUSE_MIN_SIZE = 8;

class BspListener : public ITCODBspCallback {
private:
	Map &map;
	int roomNum;
	int lastx, lasty;
public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		if (node->isLeaf()) {
			int x, y, w, h;
			TCODRandom *rng = TCODRandom::getInstance();
			w = rng->getInt(HOUSE_MIN_SIZE, node->w - 2);
			h = rng->getInt(HOUSE_MIN_SIZE, node->h - 2);
			x = rng->getInt(node->x + 1, node->x + node->w - w - 1);

		}
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width*height];
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, HOUSE_MAX_SIZE, HOUSE_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
	delete[] tiles;
}

void Map::place(bool walkable, int x1, int y1, int x2, int y2) {
	if (x2 < x1) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			tiles[tilex + tiley * width].canWalk = walkable;
		}
	}
}

void Map::createBuilding(bool first, int x1, int y1, int x2, int y2) {
	place(false, x1, y1, x2, y2);
	place(true, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	if (first) {
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	}
	else {
		TCODRandom *rng = TCODRandom::getInstance();
		if (rng->getInt(0, 3) == 0) {
			engine.actors.push(new Actor((x1 + x2) / 2, (y1 + y2) / 2, 'A', TCODColor::amber));
		}
	}
}

bool Map::isWall(int x, int y) const {
	return !tiles[x + y*width].canWalk;
}

void Map::render() const {
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			TCODConsole::root->setCharBackground(x, y,
				isWall(x, y) ? darkWall : darkGround);
		}
	}
}