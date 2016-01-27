class Engine {
public :
	TCODList<Actor *> actors;
	Actor *player;
	Map *map;

	Engine(int screenWidth, int screenHeight);
	~Engine();

	void update();
	void render();
	void sendToBack(Actor *actor);

	int fovRadius;
	int screenWidth;
	int screenHeight;

	TCOD_key_t lastKey;

	enum gameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
};

extern Engine engine;