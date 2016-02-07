class Ai {
public:
	virtual void update(Actor *owner) = 0;
};

class PlayerAi : public Ai {
public:
	void update(Actor *owner);

protected:
	Actor *choseFromInventory(Actor *owner);
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int ascii);
	void showHelp();
};

class EnemyAi : public Ai {
public:
	void update(Actor *owner);

protected:
	int moveCount;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

