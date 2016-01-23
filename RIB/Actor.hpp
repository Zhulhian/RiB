class Actor {
public:
	int x, y; // Position on map
	int ch; // ASCII code in INT form
	TCODColor col; // Color

	Actor(int x, int y, int ch, const TCODColor &col);
	void render() const;
};