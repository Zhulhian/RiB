#include "main.hpp"

static const int HOUSE_MAX_SIZE = 12;
static const int HOUSE_MIN_SIZE = 8;
static const int HOUSE_MAX_ALIENS = 4;
static const int MAX_HOUSE_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private:
	Map &map;
	int roomNum;
	int lastx, lasty;
public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		if (node->isLeaf()) {
			int x, y, w, h, door;
			TCODRandom *rng = TCODRandom::getInstance();
			// We subtract 2 from the node size (house size) to ensure they 
			// do not overlap eachother and that they won't reach the map border.
			w = rng->getInt(HOUSE_MIN_SIZE, node->w - 2); // Width of room/building
			h = rng->getInt(HOUSE_MIN_SIZE, node->h - 2); // Height of room/building

			x = rng->getInt(node->x + 1, node->x + node->w - w - 1); // x coord
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1); // y coord

			door = rng->getInt(1, 4); // Direction of door.

			// Actual creation of building
			map.createBuilding(roomNum == 0, x, y, x + w - 1, y + h - 1);

			// // // // // // // // // // // // //
			// Let's create some door openings! //
			//									//
			//			   ____2___				//
			//			  |        |			//
			//			  |		   |			//
			//			1 |        | 3			//
			//			  |		   |			//
			//			  |________|			//
			//				   4				//
			//									//
			// // // // // // // // // // // // //

			switch (door) {
			case 1:
				map.place(true, true, x, y + h / 2, x, y + h / 2);
				break;
			case 2:
				map.place(true, true, x + w / 2, y, x + w / 2, y);
				break;
			case 3:
				map.place(true, true, x + w - 1, y + h / 2, x + w - 1, y + h / 2);
				break;
			case 4:
				map.place(true, true, x + w / 2, y + h - 1, x + w / 2, y + h -1);
				break;
			}

			roomNum++;
		}
		return true;
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width*height];
	map = new TCODMap(width, height);
	map->clear(true, true);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, HOUSE_MAX_SIZE, HOUSE_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
	delete [] tiles;
	delete map;
}

void Map::addItem(int x, int y) {
	Actor *healingCap = new Actor(x, y, '\07', "healing capsule", TCODColor::lightRed);

	healingCap->blocks = false;
	healingCap->pickable = new Healer(4);
	engine.actors.push(healingCap);
}

void Map::addAlien(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int alienSelection = rng->getInt(0, 100);

	if (alienSelection < 30) {
		Actor *slogburth = new Actor(x, y, 'S', "Slogburth", TCODColor::azure);
		slogburth->destructible = new EnemyDestructible(8, 3, "pungent slimy mess");
		slogburth->attacker = new Attacker(3);
		slogburth->ai = new EnemyAi();
		engine.actors.push(slogburth);
	}
	else if (alienSelection > 30 && alienSelection < 50) {
		Actor *chloropod = new Actor(x, y, 'C', "Chloropod", TCODColor::darkCrimson);
		chloropod->destructible = new EnemyDestructible(12, 1, "pile of legs and goo");
		chloropod->attacker = new Attacker(4);
		chloropod->ai = new EnemyAi();
		engine.actors.push(chloropod);
	}
	else /*if (alienSelection > 50 && alienSelection < 70)*/{
		Actor *klamarian = new Actor(x, y, 'K', "Klaramarian", TCODColor::yellow);
		klamarian->destructible = new EnemyDestructible(7, 4, "yellow klamarian corpse");
		klamarian->attacker = new Attacker(5);
		klamarian->ai = new EnemyAi();
		engine.actors.push(klamarian);
	}
	/*
	else if (alienSelection > 70 && alienSelection < 80) {
		engine.actors.push(new Actor(x, y, 'O', "Octi-ape", TCODColor::copper));
	}
	else if (alienSelection == 30) {
		engine.actors.push(new Actor(x, y, 'Q', "Quantum Weather Butterfly", TCODColor::purple));
	}
	else if (alienSelection == 50) {
		engine.actors.push(new Actor(x, y, 'A', "Atraxi Officer", TCODColor::white));
	}
	else if (alienSelection == 70) {
		engine.actors.push(new Actor(x, y, 'T', "Time Lord", TCODColor::desaturatedSky));
	}
	else {
		engine.actors.push(new Actor(x, y, 'Y', "Yaka", TCODColor::lighterGrey));
	}
	*/
}

void Map::place(bool transparent, bool walkable, int x1, int y1, int x2, int y2) {
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
			map->setProperties(tilex, tiley, transparent, walkable);
		}
	}
}

void Map::createBuilding(bool first, int x1, int y1, int x2, int y2) {
	place(false, false, x1, y1, x2, y2);
	place(true, true, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	
	if (first) {
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	}
	else {
		TCODRandom *rng = TCODRandom::getInstance();
		int nbMonsters = rng->getInt(0, HOUSE_MAX_ALIENS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);

			if (canWalk(x, y)) {
				addAlien(x, y);
			}
			nbMonsters--;
		}

		int nbItems = rng->getInt(0, MAX_HOUSE_ITEMS);
		while (nbItems > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addItem(x, y);
			}
			nbItems--;
		}
	}
}

bool Map::canWalk(int x, int y) const {
	if (isWall(x, y)) {
		return false;
	}
	for (Actor **iterator = engine.actors.begin();
	iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}

	if (map->isInFov(x, y)) {
		tiles[x + y*width].explored = true;
		return true;
	}
	return false;
}

void Map::computeFov() {
	map->computeFov(engine.player->x, engine.player->y,
		engine.fovRadius);
}

void Map::render() const {
	static const TCODColor lightWallBg(173, 173, 173);
	static const TCODColor lightGroundBg = TCODColor::darkSea;//(90, 170, 80);
	
	static const TCODColor lightWallFg(183, 183, 183);
	static const TCODColor lightGroundFg(113, 193, 103);

	static const TCODColor darkWallBg(153, 153, 153);
	static const TCODColor darkGroundBg = TCODColor::darkerSea;//(50, 130, 40);

	static const TCODColor darkWallFg(163, 163, 163);
	static const TCODColor darkGroundFg(60, 140, 50);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (isInFov(x, y)) {
				if (isWall(x, y)) {
					TCODConsole::root->setCharBackground(x, y,
						lightWallBg);
					TCODConsole::root->setCharForeground(x, y,
						lightWallFg);
					TCODConsole::root->setChar(x, y, tiles[x + y * width].ch = TCOD_CHAR_BLOCK1);
				}
				else {
					TCODConsole::root->setCharBackground(x, y,
						lightGroundBg);
					TCODConsole::root->setCharForeground(x, y,
						lightGroundFg);
					TCODConsole::root->setChar(x, y, tiles[x + y * width].ch = '\159');
				}
			}
			else if (isExplored(x, y)) {
				if (isWall(x, y)) {
					TCODConsole::root->setCharBackground(x, y,
						darkWallBg);
					TCODConsole::root->setCharForeground(x, y,
						darkWallFg);
					TCODConsole::root->setChar(x, y, tiles[x + y * width].ch = '\112');
				}
				else {
					TCODConsole::root->setCharBackground(x, y,
						darkGroundBg);
					TCODConsole::root->setCharForeground(x, y,
						darkGroundFg);
					TCODConsole::root->setChar(x, y, tiles[x + y * width].ch = '\159');
				}
			}
		}
	}
}