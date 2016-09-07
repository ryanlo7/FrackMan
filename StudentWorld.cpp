#include "StudentWorld.h"
#include <string>
#include <queue>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
double StudentWorld::distBetweenActors(int x1, int y1, int x2, int y2)
{
	return sqrt((x1-x2) * (x1-x2) + (y1 - y2) * (y1 - y2));
}

Actor* StudentWorld::get(int x, int y)
{
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if (*it == nullptr)
			continue;
		if ((*it)->getX() == x && (*it)->getY() == y)
			return *it;
	}
	return nullptr;
}
Actor* StudentWorld::get(int x, int y, bool s)
{
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if (*it == nullptr)
			continue;
		
		if ((*it)->canPassThrough() == s)
		{
			if (x == (*it)->getX() && y == (*it)->getY() )
				return *it;
		}
	}
	return nullptr;
}
bool StudentWorld::boulderCollisions(int x, int y)
{
	bool boolVal = false;
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if (*it != nullptr)
		{
			if (distBetweenActors(x, y, (*it)->getX(), (*it)->getY()) <= 3.00)
			{
					(*it)->annoy(100);
			}
			
		}
	}
	if (distBetweenActors(x, y, frackman->getX(), frackman->getY()) <= 3.00)
	{
		frackman->annoy(100);
		return true;
	}
	return boolVal;
}
bool StudentWorld::pathCollisionDetection(int x, int y, bool s, double d)
{
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if ((*it)->canPassThrough() == s)
		{
			if (distBetweenActors((*it)->getX(), (*it)->getY(), x, y) <= d)
				return true;
		}
	}
	return false;
}
bool StudentWorld::overlap(Actor* a, int xCoord, int yCoord) //check this to see if map properly
{
	if (a == nullptr)
		return false;
	for (int x = 0; x < SPRITE_WIDTH; x++)
	{
		for (int y = 0; y < SPRITE_HEIGHT; y++)
		{
			if (a->getX() + x == xCoord && a->getY() + y == yCoord)
				return true;
		}
	}
	return false;
}
void StudentWorld::removeDirt(int xCoord, int yCoord) // removes dirt objects overlapping player
{
	for (int x = 0; x < SPRITE_WIDTH; x++)
	{
		for (int y = 0; y < SPRITE_HEIGHT; y++)
		{
			Dirt* d = dirtGrid[xCoord + x][yCoord + y];
			if (d != nullptr)
			{
				delete d;
				dirtGrid[xCoord + x][yCoord + y] = nullptr;
				playSound(SOUND_DIG);
			}
		}
	}
}
StudentWorld::~StudentWorld()
{
	delete frackman;
	for (int r = 0; r < VIEW_HEIGHT; r++)
	{
		for (int c = 0; c < VIEW_WIDTH; c++)
		{
			Dirt* d = dirtGrid[r][c];
			delete d;
		}
	}
	deleteAllActors();
}
void StudentWorld::gridNull()
{
	for (int r = 0; r < VIEW_HEIGHT; r++)
	{
		for (int c = 0; c < VIEW_WIDTH; c++)
		{
			dirtGrid[r][c] = nullptr;
			
		}
	}

}
void StudentWorld::resetMap(int a[][64])
{
	for (int r = 0; r < 64; r++)
	{
		for (int c = 0; c < 64; c++)
		{
			if (isEmptySpace(r, c))
				a[r][c] = -2;
		}
	}

}

void StudentWorld::setUpBoulders()
{
	//int B = min(current_level_number / 2 + 2, 6)
	int B = getLevel() / 2 + 2;
	
	if (B > 6)
		B = 6;
	for (int i = 0; i < B; i++)
	{
		int x, y;
		do {
			x = rand() % (VIEW_WIDTH - 3); //0 to 61
			y = rand() % (37) + 20; //20 to 56
		} while ( (x >= 27 && x <= 33) || !isAppropriateDist(x,y) );
		Boulder* b = new Boulder(this, x, y);
		actors.push_back(b);
		removeDirt(x, y);
	}
}
void StudentWorld::setUpBarrels()
{
	for (int i = 0; i < numBarrels; i++)
	{
		int x, y;
		do {
			x = rand() % (VIEW_WIDTH - 3); //0 to 60
			y = rand() % (VIEW_HEIGHT - 7); //0 to 56
		} while ((x >= 27 && x <= 33) || !isAppropriateDist(x, y));
		Barrel* b = new Barrel(this, x, y);
		actors.push_back(b);
	}
}
void StudentWorld::setUpGoldNuggets()
{
	int G = 5 - getLevel() / 2;
	if (G < 2)
		G = 2;
	for (int i = 0; i < G; i++)
	{
		int x, y;
		do {
			x = rand() % (VIEW_WIDTH - 3); //0 to 60
			y = rand() % (VIEW_HEIGHT - 7); //0 to 56
		} while ((x >= 27 && x <= 33) || !isAppropriateDist(x, y));
		GoldNugget* g = new GoldNugget(this, x, y, true);
		actors.push_back(g);
	}
	
}
void StudentWorld::deleteAllActors()
{
	list<Actor*>::iterator it;
	it = actors.begin();
	while (actors.end() != it )
	{
		if ((*it)->canMove() && (*it)->isOnlyForFrackman())
			numProtesters--;
		delete *it;
		it = actors.erase(it); //chceck if need it++
	}
}

void StudentWorld::updateMap(int a[][64], int sr, int sc)
{
	resetMap(a);
	queue<Coord>coordQueue;
	coordQueue.push(Coord(sr,sc));
	a[sr][ sc] = 0;
	//int dist = 0;
	while (coordQueue.empty() == false)
	{
		//dist++;
		Coord front = coordQueue.front();
		coordQueue.pop();
		int x = front.r(), y = front.c();

		if (x-1 >= 0  && a[x - 1][y] == -2) //left
		{
			//a[x - 1][y] = dist;
			a[x - 1][y] = 1 + a[x][y];
			coordQueue.push(Coord(x - 1, y));
		}
		if (x + 1 <= 60  && a[x + 1][y] == -2) //right
		{
			//a[x + 1][y] = dist;
			a[x + 1][y] = 1 + a[x][y];
			coordQueue.push(Coord(x + 1, y));
		}
		if (y + 1 <= 60 && a[x][y + 1] == -2) //up
		{
			//a[x][y + 1] = dist;
			a[x][y + 1] = 1 + a[x][y];
			coordQueue.push(Coord(x, y + 1));
		} 
		if (y - 1 >= 0 && a[x][y - 1] == -2) //up
		{
			/*a[x][y - 1] = dist;*/
			a[x][y - 1] = a[x][y] + 1;
			coordQueue.push(Coord(x, y - 1));
		}
	}
}
void StudentWorld::getExit(int x, int y, int& xChange, int& yChange)
{
	xChange = 0;
	yChange = 0;
	
	int up = 1000, down = 1000, left = 1000, right = 1000;
	if (x + 1 <= 60 && exitHeatMap[x + 1][y] >= 0)
		right = exitHeatMap[x + 1][y];
	if (x - 1 >= 0 && exitHeatMap[x - 1][y] >= 0)
		left = exitHeatMap[x - 1][y];
	if (y + 1 <= 60 && exitHeatMap[x][y + 1] >= 0)
		up = exitHeatMap[x][y + 1];
	if (y - 1 >= 0 && exitHeatMap[x][y - 1] >= 0)
		down = exitHeatMap[x][y - 1];
	int minimum = min(right, min(left, min(up, down)));
	if (minimum == right)
		xChange = 1;
	else if (minimum == left)
		xChange = -1;
	else if (minimum == down)
		yChange = -1;
	else if (minimum == up)
		yChange = 1;
}
int StudentWorld::getPathToFrackman(int x, int y, int& xChange, int& yChange)
{
	xChange = 0;
	yChange = 0;

	int up = 1000, down = 1000, left = 1000, right = 1000;
	if (x + 1 <= 60 && frackmanHeatMap[x + 1][y] >= 0)
		right = frackmanHeatMap[x + 1][y];
	if (x - 1 >= 0 && frackmanHeatMap[x - 1][y] >= 0)
		left = frackmanHeatMap[x - 1][y];
	if (y + 1 <= 60 && frackmanHeatMap[x][y + 1] >= 0)
		up = frackmanHeatMap[x][y + 1];
	if (y - 1 >= 0 && frackmanHeatMap[x][y - 1] >= 0)
		down = frackmanHeatMap[x][y - 1];
	int minimum = min(right, min(left, min(up, down)));
	if (minimum == right)
		xChange = 1;
	else if (minimum == left)
		xChange = -1;
	else if (minimum == down)
		yChange = -1;
	else if (minimum == up)
		yChange = 1;
	return frackmanHeatMap[x + xChange][y + yChange];
}
void StudentWorld::spawnProtesters()
{
	if (numProtesters == P)
		return;
	if (T <= spawnTicksPassed)
	{
		spawnTicksPassed = 0;
		numProtesters++;
		int hardCoreProb = getLevel()*10 + 30;
		if (hardCoreProb > 90)
			hardCoreProb = 90;
		
		int r = rand() % 100 ; //0  to 99
		if (r >= hardCoreProb)
		{
			RegularProtester* p = new RegularProtester(this, 60, 60);
			actors.push_back(p);
		}
		else {
			
			HardcoreProtester* hc = new HardcoreProtester(this, 60, 60);
			actors.push_back(hc);
			
		}
		
		
	}
	else
		spawnTicksPassed++;
}
void StudentWorld::createActor(Actor* a)
{
	actors.push_back(a);
}
void StudentWorld::setUpMap(int a[][64])
{
	for (int r = 0; r < 64; r++)
		for (int c = 0; c < 64; c++)
			a[r][c] = -1;
}
void StudentWorld::spawnCollectibles()
{
	int G = getLevel() * 25 + 300; 
	int r = rand() % G;
	if (r == 0)
	{
		r = rand() % 5;
		if (r == 0)
		{
			createActor(new Sonar(this, 0, 60));
		}
		else
		{
			int x, y;
			do {
				x = rand() % (VIEW_WIDTH - 3); // 0 to 60
				y = rand() % (VIEW_HEIGHT - 3); // 0 to 60
			} while (dirtGrid[x][y] != nullptr || !isEmptySpace(x,y)); 
			createActor(new WaterPool(this, x, y));
		}
	}
}
bool StudentWorld::isEmptySpace(int x, int y)
{
	if (x >= VIEW_WIDTH || x < 0 || y >= VIEW_HEIGHT || y < 0)
		return false;
	for (int r = 0; r < SPRITE_WIDTH; r++)
	{
		for (int c = 0; c < SPRITE_HEIGHT; c++)
		{
			if (x + r >= VIEW_WIDTH || x + r < 0 || y + c >= VIEW_HEIGHT || y + c < 0)
				return false;
			if (dirtGrid[x + r][y + c] != nullptr)
			{
				return false;
			}
		}
	}
	if (pathCollisionDetection(x, y, false, 3.00))
	{
		return false;
	}
	return true;
}
void StudentWorld::updateAllActors()
{
	list<Actor*>::iterator it;
	it = actors.begin();
	while (it != actors.end())
	{
		if ((*it)->getAlive() == false)
		{
			if ((*it)->canMove() && (*it)->isOnlyForFrackman())
				numProtesters--;
			delete *it; 
			it = actors.erase(it);
		}
		else
		{
			(*it)->doSomething();
			it++;
		}
			
	}
}
void StudentWorld::setAllVisible(int x, int y)
{
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if (distBetweenActors(x, y, (*it)->getX(), (*it)->getY()) <= 12.00)
			(*it)->setVisible(true);
	}
}
string StudentWorld::editDisplayText(int score, int level, int lives, int health, int water, int gold, int sonar, int oil)
{ 
	// Scr:	321000  Lvl: 52	 Lives:	3  Hlth: 80%  Wtr: 20  Gld:	3  Sonar: 1	 Oil Left: 2
	
	
	string title = "Scr: ";
	if (score > 999999)
		score = 999999;
	string scoreNum = to_string(score);
	for (int i = 0; i < 6 - (int)scoreNum.size(); i++)
		title += "0";
	title += scoreNum + "  Lvl: ";
	if (level / 10 <= 0)
		title += " ";
	title += to_string(level) + "  Lives: " + to_string(lives) +
		"  Hlth: "; 
	if (health < 10)
		title += " ";
	title += to_string(health * 10) + "%  Wtr: ";
	if (water / 10 <= 0)
		title += " ";
	title += to_string(water) + "  Gld: ";
	if (gold / 10 <= 0)
		title += " ";
	title += to_string(gold) + "  Sonar: ";
	if (sonar / 10 <= 0)
		title += " ";
	title += to_string(sonar) + "  Oil Left: ";
	if (oil / 10 <= 0)
		title += " ";
	title += to_string(oil);

	return title;
}
void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = frackman->getHealth();
	int water = frackman->getWater();
	int gold = frackman->getNuggets();
	int sonar = frackman->getSonar();
	int oil =  numBarrels - frackman->getOil();
	
	string s = editDisplayText(score, level, lives, health, water, gold, sonar, oil);
	setGameStatText(s);
}

bool StudentWorld::protesterInteractions( Actor* target)
{
	bool value = false;
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* a = *it;
		if (a->canMove() && a->isOnlyForFrackman() )
		{
			if (distBetweenActors(a->getX(), a->getY(), target->getX(), target->getY() ) <= 3.00)
			{

				if (target->canMove() ) //if water squirt
				{
					a->annoy(2);
					value = true;
				}
				else if (!target->isOnlyForFrackman() ) //if gold nugget
				{
					a->bribe() ;
					return true;
				}
			}
		}
	}
	return value;
}
void StudentWorld::setUpGrid()
{
	gridNull();
	for (int x = 0; x < VIEW_WIDTH; x++)
	{
		for (int y = 0; y < VIEW_HEIGHT; y++)
		{
			if (y < 60 && (x > 33 || x < 30))
			{
				dirtGrid[x][y] = new Dirt(this, x, y);
			}
			else if ((x <= 33 && x >= 30) && (y < 4))
			{
				dirtGrid[x][y] = new Dirt(this, x, y);
			}
		}
	}
}
Dirt* StudentWorld::getDirt(int x, int y)
{
	return dirtGrid[x][y];
}
bool StudentWorld::isAppropriateDist(int x, int y)
{
	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		if (distBetweenActors(x, y, (*it)->getX(), (*it)->getY()) <= 6.0)
			return false;
	}
	return true;
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
