#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Actor : public GraphObject
{
public:

	Actor(int imageID, int x, int y, Direction dir, double s, unsigned int depth, StudentWorld* w);
	virtual ~Actor() {}
	virtual void doSomething() = 0;
	StudentWorld* getWorld() { return m_world; }
	bool getAlive() { return alive; }
	void setAlive(bool b) { alive = b; }
	virtual void annoy(int n) { } ;
	virtual bool canMove() { return false; }
	virtual bool isTemporary() { return false; }
	virtual bool isOnlyForFrackman() { return true; }
	virtual bool canPassThrough() { return true; }
	virtual void bribe() { }

private:
	StudentWorld* m_world;
	bool alive;
};

class Dirt : public Actor  
{
public:
	Dirt(StudentWorld* w, int x, int y);
	virtual ~Dirt();
	virtual void doSomething();
	//virtual void annoy(int n) { }
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* w, int x, int y);
	virtual ~Boulder();
	virtual void doSomething();
	//virtual void annoy(int n) { }
	bool getStable() { return stable; }
	void setStable(bool b) { stable = b; }
	virtual bool canPassThrough() { 
		return false;
	}

private:
	int waitingTicks;
	bool boulderStillFalling();
	void stableActions();
	bool stable;
};

class Goodie : public Actor
{
public:
	Goodie(StudentWorld* w, int imageID, int x, int y);
	virtual ~Goodie() { };
	int getTicks() { return numTicks; }
	void setTicks(int n) { numTicks = n; }
	bool isHidden() { return hidden; };
	void setHidden(bool b) { hidden = b; }
	bool getState() { return permanentState; }
	void setState(bool b) { permanentState = b; }
	
	
private:
	int numTicks;
	bool hidden;
	bool permanentState;
};

class GoldNugget : public Goodie
{
public:
	GoldNugget(StudentWorld* w, int x, int y, bool s);
	virtual ~GoldNugget();
	virtual void doSomething();
	//virtual void annoy(int n) { }
	virtual bool isOnlyForFrackman() { return getState(); }
};
class Sonar : public Goodie
{
public:
	Sonar(StudentWorld* w, int x, int y);
	virtual ~Sonar();
	//virtual void annoy(int n) { }
	virtual void doSomething();
private:
	int maxTicks;
};

class WaterPool : public Goodie
{
public:
	WaterPool(StudentWorld* w, int x, int y);
	virtual ~WaterPool();
	//virtual void annoy(int n) { }
	virtual void doSomething();
private:
	int maxTicks;
};

class Barrel : public Goodie
{
public:
	Barrel(StudentWorld* w, int x, int y);
	virtual ~Barrel();
	//virtual void annoy(int n) { }
	virtual void doSomething();
};
class Squirt : public Actor
{
public:
	Squirt(StudentWorld*, int x, int y, Direction d);
	virtual ~Squirt();
	//virtual void annoy(int n) { };
	virtual void doSomething();
	virtual bool canMove() { return true; }
private:
	const int travelDist = 4;
	int distTraveled;

};
class Mover : public Actor
{
public:
	Mover(StudentWorld* w, int imageID, int x, int y, Direction dir);
	virtual ~Mover();
	int getHealth();
	void setHealth(int h);
	virtual bool canMove() { return true; }
	bool hittingBoulder(int x, int y);
private:
	int health;
};
class Protester : public Mover
{
public:
	Protester(StudentWorld* w, int x, int y, int id);
	virtual ~Protester() { }
	virtual void annoy(int n);

	void setQuit(bool b) { quit = b; }
	bool getQuit() { return quit; }
	void giveUp();
	bool move(int x, int y);
	bool isFacingFrackman();
	bool faceFrackman(Direction& d);
	bool isIntersection(Direction& d);
	bool pickNewDirection();

	int getFreezeTime() { return freezeTime; }
	void setFreezeTime(int n) { freezeTime = n; }
	/*void setFreezeTimePassed(int n) { freezeTicksPassed = n; }
	int getFreezeTimePassed() { return freezeTicksPassed; }*/
	void setSteps(int n) { numStepsToMove = n; }
	int getSteps() { return numStepsToMove; }
	int getTicks() { return ticksToWait; }
	void setTicks(int n) { ticksToWait = n; }
	void setTicksPassed(int n) { waitingTicksPassed = n; }
	int getTicksPassed() { return waitingTicksPassed; }
	int getPerpTicks() { return numPerpendicularTicks; }
	void setPerpTicks(int n) { numPerpendicularTicks = n; }
private:
	int freezeTime; //non waiting ticks
	//int freezeTicksPassed;
	bool upPath(int x, int y);
	bool downPath(int x, int y);
	bool rightPath(int x, int y);
	bool leftPath(int x, int y);
	int numStepsToMove;
	int ticksToWait;
	int waitingTicksPassed;
	bool quit;

	int numPerpendicularTicks;
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* w, int x, int y);
	virtual ~RegularProtester() { setVisible(false); }
	virtual void doSomething();
	virtual void bribe();
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* w, int x, int y);
	virtual ~HardcoreProtester() { setVisible(false); }
	virtual void doSomething();
	virtual void bribe();
	virtual void annoy(int n);
};

class Frackman : public Mover
{
public:
	Frackman(StudentWorld* w, int x, int y);
	virtual ~Frackman();
	virtual void annoy(int n);
	int getWater();
	void setWater(int n);
	int getNuggets();
	void setNuggets(int n);
	int getSonar();
	void setSonar(int n);
	int getOil();
	void setOil(int n);
	virtual bool isOnlyForFrackman() { return false; }
	virtual void doSomething();
private:
	int numNuggets;
	int numWater;
	int numSonar;
	int numOil;

};


#endif // ACTOR_H_
