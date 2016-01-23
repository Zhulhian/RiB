struct Tile 
{
	bool canWalk; // Can we walk through this tile?
	Tile() : canWalk(true) {}
};

class Map {
public :
	int width, height;

	Map(int width, int height);
	~Map();
	bool isWall(int x, int y) const;
	void render() const;
protected :
	Tile *tiles;
	friend class BspListener;

	void place(bool walkable, int x1, int y1, int x2, int y2);
	void createBuilding(bool first, int x1, int y1, int x2, int y2);
};