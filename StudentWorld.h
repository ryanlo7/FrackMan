#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <list>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:

	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
		frackman = nullptr;
		gridNull();
		
	}
	virtual int init()
	{
		setUpMap(exitHeatMap);
		numBarrels = getLevel() + 2;
		if (numBarrels > 20)
			numBarrels = 20;
		
		T = 200 - getLevel();
		if (T < 25)
			T = 25;
		spawnTicksPassed = T;
		P = (int ) (2 + getLevel() * 1.5);
		if (P > 15)
			P = 15;
		numProtesters = 0;

		setUpGrid();
		frackman = new Frackman(this, 30, 60);
		setUpBoulders();
		setUpGoldNuggets();
		
		setUpBarrels();
		return GWSTATUS_CONTINUE_GAME;
	}
	
	virtual int move()
	{
		setDisplayText();
		spawnCollectibles();
		spawnProtesters();
		updateAllActors();

		frackman->doSomething();
		if (!frackman->getAlive())
		{
			decLives();
			delete frackman;
			frackman = nullptr;
			return GWSTATUS_PLAYER_DIED;
		}
		updateMap(exitHeatMap, 60, 60);
		updateMap(frackmanHeatMap, frackman->getX(), frackman->getY());

		if (numBarrels - frackman->getOil() == 0)
		{
			return GWSTATUS_FINISHED_LEVEL;
		}
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		delete frackman;
		frackman = nullptr;
		for (int r = 0; r < VIEW_HEIGHT; r++)
		{
			for (int c = 0; c < VIEW_WIDTH; c++)
			{
				Dirt* d = dirtGrid[r][c];
				delete d;
				dirtGrid[r][c] = nullptr;
			}
		}
		deleteAllActors();
	}
	
	~StudentWorld();
	bool overlap(Actor* a, int x, int y);
	Dirt* getDirt(int x, int y);
	void removeDirt(int x, int y);
	Actor* get(int x, int y);
	Actor* get(int x, int y, bool s);
	bool boulderCollisions(int x, int y);
	Frackman* getFrackman() { return frackman; }
	double distBetweenActors(int x1, int y1, int x2, int y2);
	bool pathCollisionDetection(int x, int y, bool state, double d);
	void createActor(Actor* a);
	void setAllVisible(int x, int y);
	void spawnCollectibles();
	bool isEmptySpace(int x, int y);
	bool protesterInteractions(Actor* a);
	void getExit(int x, int y, int& xChange, int& yChange);
	int getPathToFrackman(int x, int y, int& xChange, int& yChange);
	bool isInGrid(int x, int y)
	{
		if (x >= VIEW_WIDTH || x < 0 || y >= VIEW_HEIGHT || y < 0)
			return false;
		return true;
	}

private:

	class Coord
	{
	public:
		Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
		int r() const { return m_r; }
		int c() const { return m_c; }
	private:
		int m_r;
		int m_c;
	};

	void gridNull();
	Dirt* dirtGrid[VIEW_HEIGHT][VIEW_WIDTH];
	int frackmanHeatMap[VIEW_HEIGHT][VIEW_WIDTH];
	int exitHeatMap[VIEW_HEIGHT][VIEW_WIDTH ];
	void resetMap(int a[][64]);
	void updateMap(int a[][64], int sr, int sc);
	void setUpMap(int a[][64]);
	Frackman* frackman;
	void setUpGrid();
	void setUpBoulders();
	void setUpGoldNuggets();
	void setUpBarrels();
	void spawnProtesters();
	std::list<Actor*> actors;
	std::string editDisplayText(int score, int level, int lives, int health,
		int water, int gold, int sonar, int oil);

	bool isAppropriateDist(int x, int y);
	void updateAllActors();
	void deleteAllActors();
	void setDisplayText();	
	
	int numBarrels;
	int T;
	int P;
	int numProtesters;
	int spawnTicksPassed;
};

#endif // STUDENTWORLD_H_
