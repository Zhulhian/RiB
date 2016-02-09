class Actor {
public:
	int x, y;			// Position on map
	int ch;				// ASCII code in INT form
	const char *name;	// Name
	TCODColor colFg;	// Color
	TCODColor colBg;

	bool blocks;
	bool dead;

	Attacker *attacker; // Something that can attack.
	Destructible *destructible; // Something that can die.
	Ai *ai; // Something self-updating
	Pickable *pickable; // Something that can be picked up and used.
	Container *container; // Something that can contain / A c t o r s /.

	Actor(int x, int y, int ch, const char *name, const TCODColor &colFg, const TCODColor &colBg = TCODColor::amber);
	~Actor();

	void update();
	void render() const;
};