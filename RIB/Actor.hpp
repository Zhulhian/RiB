class Actor {
public:
	int x, y;			// Position on map
	int ch;				// ASCII code in INT form
	const char *name;	// Name
	TCODColor col;		// Color

	bool blocks;

	Attacker *attacker;
	Destructible *destructible;
	Ai *ai;

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);

	void update();
	void render() const;
};