class Spritesheet {
public:
	TCODImage *spritesheet;

	
	std::tuple<int, int> playerPortrait;
	

	Spritesheet(const char* file);
	~Spritesheet();
};