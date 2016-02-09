struct Tile 
{
	bool explored; // Have we seen this tile?
	unsigned char ch;
	TCODColor colBg;
	TCODColor colFg;

	Tile() : explored(false), ch('"'), colBg(TCODColor::darkTurquoise), colFg(colBg * 1.1f) {}
};

class Map {
public :
	int width, height;

	Map(int width, int height);
	~Map();

	bool canWalk(int x, int y) const;
	bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;

	void computeFov();
	void render() const;
protected :
	Tile *tiles;
	TCODMap *map;
	friend class BspListener;

	void place(bool transparent, bool walkable, int x1, int y1, int x2, int y2, char tileType = '"', TCODColor colBg = TCODColor::amber, TCODColor colFg = TCODColor::lightYellow);
	void createBuilding(bool first, int x1, int y1, int x2, int y2);
	void addAlien(int x, int y);
	void addItem(int x, int y);
};