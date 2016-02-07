class Actor {
public:
	int x, y;			// Position on map
	int ch;				// ASCII code in INT form
	const char *name;	// Name
	TCODColor col;		// Color

	bool blocks;

	Attacker *attacker; // Something that can attack.
	Destructible *destructible; // Something that can die.
	Ai *ai; // Something self-updating
	Pickable *pickable; // Something that can be picked up and used.
	Container *container; // Something that can contain / A c t o r s /.

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	~Actor();

	void update();
	void render() const;
};