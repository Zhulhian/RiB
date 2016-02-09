class Destructible {
public:
	float maxHp;
	float hp;
	float defense;
	const char *corpseName;
	int corpseTile;

	Destructible(float maxHp, float defense, const char *corpseName, int corpseTile);

	inline bool isDead() { return hp <= 0; }
	float takeDamage(Actor *owner, float damage);
	float heal(float amount);

	virtual void die(Actor *owner);
};

class EnemyDestructible : public Destructible {
public: 
	EnemyDestructible(float maxHp, float defense, const char *corpseName, int corpseTile);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName, int corpseTile);
	void die(Actor *owner);
};

class ObjectDestructible : public Destructible {
public:
	ObjectDestructible(float maxHp, float defense, const char *brokenName, int brokenTile);
	void die(Actor *owner);
};
