#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int imageID, int x, int y, Direction dir, double s, unsigned int depth, StudentWorld* w)
	: GraphObject(imageID, x, y, dir, s, depth)
{ 
	m_world = w;
	alive = true;
}

Dirt::Dirt(StudentWorld* w, int x, int y) :
	Actor(IID_DIRT, x, y, right, 0.25, 3, w)
{
	setVisible(true);
}
Dirt::~Dirt()
{
	setVisible(false);
}
void Dirt::doSomething() { }

Boulder::Boulder(StudentWorld* w, int x, int y) :
	Actor(IID_BOULDER, x, y, down, 1, 1, w)
{
	setVisible(true);
	setStable(true);
	waitingTicks = 30;
}
Boulder::~Boulder()
{
	setVisible(false);
}
void Boulder::doSomething()
{
	if (!getAlive())
		return;
	if (getStable() )
	{
		stableActions();
	}
	else if (waitingTicks > 0)
	{
		waitingTicks--;
		if (waitingTicks <= 0)
			getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	else 
	{
		if (!boulderStillFalling() )
			return;
		if (getAlive())
		{
			getWorld()->boulderCollisions(getX(), getY()); // aim to annoy
			moveTo(getX(), getY() - 1);
		}
	}
}

void Boulder::stableActions()
{
	for (int i = 0; i < SPRITE_WIDTH; i++)
	{
		Dirt* d = getWorld()->getDirt(getX() + i, getY() - 1);
		if (d != nullptr)
			return;
	}
	setStable(false);
}
bool Boulder::boulderStillFalling()
{
	if (getY() <= 0)
	{
		setAlive(false);
		return false;
	}
	for (int i = 0; i < SPRITE_WIDTH; i++)
	{
		if (getWorld()->getDirt(getX() + i, getY() - 1))
		{
			setAlive(false);
			return false;
		}
		Actor* a = getWorld()->get(getX() + i, getY() - 1, false);
		if (a != nullptr) // check for boulder hitting another boulder
		{
			if (getY() - 1 == a->getY() + 3 &&
				(getX() + i == a->getX() || getX() + i == a->getX() + 3))
			{
				setAlive(false);
				return false;
			}
		}
	}
	
	return true;
}

Goodie::Goodie(StudentWorld* w, int imageID, int x, int y) :
	Actor(imageID, x, y, right, 1.0, 2, w)
{
	setState(true);
	numTicks = 0;
}

GoldNugget::GoldNugget(StudentWorld* w, int x, int y, bool state) :
	Goodie(w, IID_GOLD, x, y)
{
	setState(state);
	if (state) 
	{
		setVisible(false);
		setHidden(true);
	}
	else
	{
		setVisible(true);
		setHidden(false);
	}
}
GoldNugget::~GoldNugget()
{
	setVisible(false);
}
void GoldNugget::doSomething()
{
	if (getAlive() == false)
		return;
	Frackman* f = getWorld()->getFrackman();
	if (getState()  && isHidden() ) //if permanent state and hidden
	{
		if (getWorld()->distBetweenActors(
			getX(), getY(), f->getX(), f->getY()) <= 4.00 )
		{
			setVisible(true); //appear
			setHidden(false);
			return;
		}
	}
	else if (getState() && getWorld()->distBetweenActors(getX(), getY(), 
			f->getX(), f->getY() ) <= 3.00)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		f->setNuggets(f->getNuggets() + 1);
	}
	else if (!getState() )
	{
		if (getWorld()->protesterInteractions(this) )
		{
			setAlive(false);
			getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		}

		setTicks(getTicks() + 1);
		if (getTicks() >= 100)
			setAlive(false);
	}
}

Sonar::Sonar(StudentWorld* w, int x, int y) :
	Goodie(w, IID_SONAR, x, y)
{
	setVisible(true);
	maxTicks = 300 - 10 * getWorld()->getLevel();
	if (maxTicks < 100)
		maxTicks = 100;
	setState(true);
}
void Sonar::doSomething()
{
	if (!getAlive())
		return;
	Frackman* f = getWorld()->getFrackman();
	if (getWorld()->distBetweenActors(f->getX(), f->getY(), getX(), getY()) <= 3.0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		f->setSonar(f->getSonar() + 1);
		getWorld()->increaseScore(75);
	}
	setTicks(getTicks() + 1);
	if (getTicks() >= maxTicks)
		setAlive(false);
}
Sonar::~Sonar()
{
	setVisible(false);
}

WaterPool::WaterPool(StudentWorld* w, int x, int y) :
	Goodie(w, IID_WATER_POOL, x, y)
{
	setVisible(true);
	setState(true);
	maxTicks = 300 - 10 * getWorld()->getLevel();
	if (maxTicks < 100)
		maxTicks = 100;
}
WaterPool::~WaterPool()
{
	setVisible(false);
}
void WaterPool::doSomething()
{
	if (!getAlive())
		return;
	Frackman* f = getWorld()->getFrackman();
	if (getWorld()->distBetweenActors(f->getX(), f->getY(), getX(), getY()) <= 3.0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		f->setWater(f->getWater() + 5);
		getWorld()->increaseScore(100);
	}
	setTicks(getTicks() + 1);
	if (getTicks() >= maxTicks)
		setAlive(false);
}

Barrel::Barrel(StudentWorld* w, int x, int y) :
	Goodie(w, IID_BARREL, x, y)
{
	setVisible(false);
	setHidden(true);
	setState(true);
}
Barrel::~Barrel()
{
	setVisible(false);
}
void Barrel::doSomething()
{
	Frackman* f = getWorld()->getFrackman();
	if (!getAlive())
		return;
	if (isHidden() && getWorld()->distBetweenActors(getX(), getY(), f->getX(), f->getY()) <= 4.00)
	{
		setVisible(true);
		setHidden(false);
		return;
	}
	else if (getWorld()->distBetweenActors(getX(), getY(), f->getX(), f->getY()) <= 3.00)
	{
		setAlive(false);
		getWorld()->increaseScore(1000);
		getWorld()->playSound(SOUND_FOUND_OIL);
		f->setOil(f->getOil() + 1);
	}
}

Squirt::Squirt(StudentWorld* w, int x, int y, Direction d) :
	Actor(IID_WATER_SPURT, x, y, d, 1.0, 1, w)
{
	setVisible(true);
	distTraveled = 0;
}
Squirt::~Squirt()
{
	setVisible(false);
}
void Squirt::doSomething()
{
	if (getWorld()->protesterInteractions(this))
	{
		setAlive(false);
	}
	if (distTraveled >= travelDist)
		setAlive(false);
	else
	{
		switch (getDirection())
		{
			case up:
				if (!getWorld()->isEmptySpace(getX(), getY() + 1)) 
					setAlive(false);
				else
					moveTo(getX(), getY() + 1);
				break;
			case right:
				if (!getWorld()->isEmptySpace(getX() + 1, getY()))
					setAlive(false);
				else
					moveTo(getX() + 1, getY());
				break;
			case down:
				if (!getWorld()->isEmptySpace(getX(), getY() - 1))
					setAlive(false);
				else
					moveTo(getX(), getY() - 1);
				break;
			case left:
				if (!getWorld()->isEmptySpace(getX() - 1, getY()))
					setAlive(false);
				else
					moveTo(getX() - 1, getY());
				break;
			default:
				break;
		}
		distTraveled++;
	}
}

Mover::Mover(StudentWorld* w, int imageID, int x, int y, Direction dir) :
	Actor(imageID, x, y, dir, 1, 0, w)
{ }
Mover::~Mover() { }
int Mover::getHealth()
{
	return health;
}
void Mover::setHealth(int h)
{
	health = h;
}
bool Mover::hittingBoulder(int x, int y)
{
	return getWorld()->pathCollisionDetection(x, y, false, 3.00);
}


Protester::Protester(StudentWorld* w, int x, int y, int id) :
	Mover(w, id, x, y, left)
{
	quit = false;
	setFreezeTime(0);
	setTicksPassed(0);
}
void Protester::annoy(int n)
{
	if (getQuit()) return;
	setHealth(getHealth() - n);
	if (getHealth() <= 0)
	{
		setQuit(true);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		//setTicks(0);
		setFreezeTime(0);
		if (n == 100)
		{
			getWorld()->increaseScore(500);
		}
		else
			getWorld()->increaseScore(100);
		return;
	}
	int N = 100 - getWorld()->getLevel() * 10;
	if (50 > N)
		N = 50;
	setFreezeTime(N);
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
}
bool Protester::move(int x, int y)
{
	if (!getWorld()->isEmptySpace(getX() + x, getY() + y))
		return false;
	if (x < 0)
		setDirection(left);
	else if (y < 0)
		setDirection(down);
	else if (x > 0)
		setDirection(right);
	else if (y > 0)
		setDirection(up);
	moveTo(getX() + x, getY() + y);
	return true;
}
void Protester::giveUp()
{
	if (getX() == 60 && getY() == 60)
	{
		setAlive(false);
		return;
	}
	int x, y;
	getWorld()->getExit(getX(), getY(), x, y);
		
	move(x, y);
}
bool Protester::isFacingFrackman()
{
	Frackman* f = getWorld()->getFrackman();

	switch (getDirection())
	{
		case right: 
			for (int i = 4; getX() + i < VIEW_WIDTH; i += SPRITE_WIDTH)
			{
				if (!getWorld()->isEmptySpace(getX() + i, getY()))
					return false;
				if (getWorld()->overlap(f, getX() + i, getY()))
					return true;
			}
			break;
		case left :
			for (int i = 4; getX() - i >= 0; i += SPRITE_WIDTH)
			{
				if (!getWorld()->isEmptySpace(getX() - i, getY()))
					return false;
				if (getWorld()->overlap(f, getX() - i, getY()))
					return true;
			}
			break;
		case down:
			for (int i = 4; getY() - i >= 0; i += SPRITE_WIDTH)
			{
				if (!getWorld()->isEmptySpace(getX(), getY() - i))
					return false;
				if (getWorld()->overlap(f, getX(), getY() - i))
					return true;
			}
			break;
		case up:
			for (int i = 4; getY() + i < VIEW_WIDTH; i += SPRITE_WIDTH)
			{
				if (!getWorld()->isEmptySpace(getX(), getY() + i))
					return false;
				if (getWorld()->overlap(f, getX(), getY() + i))
					return true;
			}
			break;
		default:
			break;
	}

	return false;
}
bool Protester::faceFrackman(Direction& dir)
{
	//up
	Frackman* f = getWorld()->getFrackman();
	for (int i = 4; getY() + i < VIEW_WIDTH; i += 1)
	{
		if (!getWorld()->isInGrid(getX(), getY() + i))
			break;
		else if (getWorld()->getDirt(getX(), getY() + i) || 
			getWorld()->get(getX(), getY() + i, false) )
			break;
		else if (f->getX() == getX() && f->getY() == getY() + i)
		//else if (getWorld()->overlap(f, getX(), getY() + i))
		{
			dir = up;
			move(0, 1);
			return true;
		}
	}

	//down
	for (int i = 1; getY() - i > 0; i += 1)
	{
		if (!getWorld()->isInGrid(getX(), getY() - i))
			break;
		else if (getWorld()->getDirt(getX(), getY() - i) ||
			getWorld()->get(getX(), getY() - i, false))
			break;
		else if (f->getX() == getX() && f->getY() == getY() - i)
		//else if (getWorld()->overlap(f, getX(), getY() - i))
		{
			dir = down;
			move(0, -1);
			return true;
		}
	}
	//right
	for (int i = 4; getX() + i < VIEW_WIDTH; i += 1)
	{
		if (!getWorld()->isInGrid(getX() + i, getY()))
			break;
		else if (getWorld()->getDirt(getX() + i, getY()) ||
			getWorld()->get(getX() + i, getY(), false))
			break;
		else if (f->getX() == getX() + i && f->getY() == getY())
		//else if (getWorld()->overlap(f, getX() + i, getY()))
		{
			dir = right;
			move(1, 0);
			return true;
		}
	}
	//left
	for (int i = 1; getX() - i > 0; i += 1)
	{
		if (!getWorld()->isInGrid(getX() - i, getY()))
			break;
		else if (getWorld()->getDirt(getX() - i, getY()) ||
			getWorld()->get(getX() - i, getY(), false))
			break;
		else if (f->getX() == getX() - i && f->getY() == getY())
		//else if (getWorld()->overlap(f, getX() - i, getY()) )
		{
			dir = left;
			move(-1, 0);
			return true;
		}
	}
	return false;
}
bool Protester::upPath(int X, int Y)
{
	for (int c = 0; c < 4; c++)
	{
		int x = X + c;
		int y = Y + 4;
		if (!getWorld()->isInGrid(x, y)
			|| getWorld()->getDirt(x, y)
			|| getWorld()->get(x, y, false))
			return false;
	}
	return true;
}
bool Protester::downPath(int X, int Y)
{
	for (int c = 0; c < 4; c++)
	{
		int x = X + c;
		int y = Y - 1;
		if (!getWorld()->isInGrid(x, y)
			|| getWorld()->getDirt(x, y)
			|| getWorld()->get(x, y, false))
			return false;
	}
	return true;
}
bool Protester::rightPath(int X, int Y)
{
	for (int r = 0; r < 4; r++)
	{
		int x = X + 4;
		int y = Y + r;
		if (!getWorld()->isInGrid(x, y)
			|| getWorld()->getDirt(x, y)
			|| getWorld()->get(x, y, false))
			return false;
	}
	return true;
}
bool Protester::leftPath(int X, int Y)
{
	for (int r = 0; r < 4; r++)
	{
		int x = X - 1;
		int y = Y + r;
		if (!getWorld()->isInGrid(x, y)
			|| getWorld()->getDirt(x, y)
			|| getWorld()->get(x, y, false))
			return false;
	}
	return true;
}
bool Protester::isIntersection(Direction& d)
{
	StudentWorld* w = getWorld();
	
	switch (d)
	{
		case up:
		case down:
			if (leftPath(getX(), getY() ))
			{
				d = left;
				return true;
			}
			if (rightPath(getX(), getY()))
			{
				d = right;
				return true;
			}
			return false;
		
		case right:
		case left:
			if (upPath(getX(), getY()))
			{
				d = up;
				return true;
			}
			if (downPath(getX(), getY()))
			{
				d = down;
				return true;
			}
			return false;
		default:
			break;
	}
	return false;
}
bool Protester::pickNewDirection()
{
	Direction d;
	bool b;
	do {
		int random = rand() % 4; // 0 to 4
		switch (random)
		{
			case 3: // up
				d = up;
				b = upPath(getX(), getY());
				break;
			case 2:
				d = right;
				b = rightPath(getX(), getY());
				break;
			case 1:
				d = down;
				b = downPath(getX(), getY());
				break;
			case 0:
				d = left;
				b = leftPath(getX(), getY());
				break;
		}
	} while (!b);

	setDirection(d);
	setSteps(8 + rand() % 53);
	return true;
}

RegularProtester::RegularProtester(StudentWorld* w, int x, int y) :
	Protester(w, x, y, IID_PROTESTER)
{
	setVisible(true);
	setHealth(5);
	setPerpTicks(0);
	int s = 8 + rand() % 53; // 8 to 60 inclusive
	setSteps(s);
	s = 3 - w->getLevel() / 4;
	if (s < 0) s = 0;
	setTicks(s);
}
void RegularProtester::bribe()
{
	setQuit(true);
	getWorld()->increaseScore(25);
}
void RegularProtester::doSomething()
{
	if (!getAlive()) return;
	if (getFreezeTime() > 0)
	{
		setFreezeTime(getFreezeTime() - 1);
		return;
	}
	Frackman* f = getWorld()->getFrackman();
	if (getTicks() > getTicksPassed())
	{
		setTicksPassed(getTicksPassed() + 1);
		return;
	}
	setTicksPassed(0);
	if (getQuit()) {
		giveUp();
		return;
	}
	setPerpTicks(getPerpTicks() - 1);
	
	double dist = getWorld()->distBetweenActors(getX(), getY(), f->getX(), f->getY());
	if (dist <= 4.00 && isFacingFrackman())
	{
		if (getFreezeTime() <= 0) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			f->annoy(2);
			setFreezeTime(15);
			return;
		}
	}
	Direction dir = getDirection();
	if (dist >= 4.00 && faceFrackman(dir))
	{
		setDirection(dir);
		setSteps(0);
		return;
	}
	else {
		setSteps(getSteps() - 1);
		bool b = true;
		switch (getDirection())
		{
		case up:
			b = move(0, 1);
			break;
		case down:
			b = move(0, -1);
			break;
		case left:
			b = move(-1, 0);
			break;
		case right:
			b = move(1, 0);
			break;
		}
		if (getSteps() <= 0 || !b)
		{
			pickNewDirection();
		}
		else if (isIntersection(dir)) {
			if (getPerpTicks() <= 0)
			{
				setDirection(dir);
				setPerpTicks(200);
				int s = 8 + rand() % 53; // 8 to 60 inclusive
				setSteps(s);
			}
		}
	}
	
}

HardcoreProtester::HardcoreProtester(StudentWorld* w, int x, int y) :
	Protester(w, x, y, IID_HARD_CORE_PROTESTER)
{
	setVisible(true);
	setHealth(20);
	setPerpTicks(0);
	int s = 8 + rand() % 53; // 8 to 60 inclusive
	setSteps(s);
	s = 3 - w->getLevel() / 4;
	if (s < 0) s = 0;
	setTicks(s);
}
void HardcoreProtester::bribe()
{
	int n = 100 - getWorld()->getLevel() * 10;
	if (50 > n)
		n = 50;
	setFreezeTime(n);
	getWorld()->increaseScore(50);
}
void HardcoreProtester::annoy(int n)
{
	if (getQuit())
		return;
	setHealth(getHealth() - n);
	if (getHealth() <= 0)
	{
		setQuit(true);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		//setTicks(0);
		setFreezeTime(0);
		if (n == 100)
		{
			getWorld()->increaseScore(500);
		}
		else
			getWorld()->increaseScore(250);
		return;
	}
	int N = 100 - getWorld()->getLevel() * 10;
	if (50 > N)
		N = 50;
	setFreezeTime(N);
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
}
void HardcoreProtester::doSomething()
{
	if (!getAlive()) return;
	if (getFreezeTime() > 0)
	{
		setFreezeTime(getFreezeTime() - 1);
		return;
	}
	Frackman* f = getWorld()->getFrackman();
	if (getTicks() > getTicksPassed())
	{
		setTicksPassed(getTicksPassed() + 1);
		return;
	}
	setTicksPassed(0);
	if (getQuit()) {
		giveUp();
		return;
	}
	setPerpTicks(getPerpTicks() - 1);

	double dist = getWorld()->distBetweenActors(getX(), getY(), f->getX(), f->getY());
	if (dist <= 4.00 && isFacingFrackman())
	{
		if (getFreezeTime() <= 0) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			f->annoy(2);
			setFreezeTime(15);
			return;
		}
	}

	Direction dir = getDirection();
	if (dist >= 4.00 && faceFrackman(dir))
	{
		setDirection(dir);
		setSteps(0);
		return;
	}
	else {
		//chase Frackman
		int xChange, yChange;
		int d = getWorld()->getPathToFrackman(getX(), getY(), xChange, yChange);
		if ((int)getWorld()->getLevel() * 2 + 16 >= d)
		{
			move(xChange, yChange);
			return;
		}

		setSteps(getSteps() - 1);
		bool b = true;
		switch (getDirection())
		{
		case up:
			b = move(0, 1);
			break;
		case down:
			b = move(0, -1);
			break;
		case left:
			b = move(-1, 0);
			break;
		case right:
			b = move(1, 0);
			break;
		}
		if (getSteps() <= 0 || !b)
		{
			pickNewDirection();
		}
		else if (isIntersection(dir)) {
			if (getPerpTicks() <= 0)
			{
				setDirection(dir);
				setPerpTicks(200);
				int s = 8 + rand() % 53; // 8 to 60 inclusive
				setSteps(s);
			}
		}
	}

}

Frackman::Frackman(StudentWorld* w, int x = 30, int y = 60) :
	Mover(w, IID_PLAYER, x, y, right)
{
	setHealth(10);
	numWater = 5;
	numNuggets = 0;
	numSonar = 1;
	numOil = 0;
	setVisible(true);
}
Frackman::~Frackman()
{
	setVisible(false);
}
void Frackman::annoy(int n)
{
	setHealth(getHealth() - n);
	if (getHealth() <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}
int Frackman::getNuggets()
{
	return numNuggets;
}
void Frackman::setNuggets(int n)
{
	numNuggets = n;
}
int Frackman::getWater()
{
	return numWater;
}
void Frackman::setWater(int n)
{
	numWater = n;
}
int Frackman::getOil()
{
	return numOil;
}
void Frackman::setOil(int n)
{
	numOil = n;
}
int Frackman::getSonar()
{
	return numSonar;
}
void Frackman::setSonar(int n)
{
	numSonar = n;
}
void Frackman::doSomething()
{
	if (getAlive() == false)
		return;
	//Remove Dirt
	getWorld()->removeDirt(getX(), getY() );

	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
			case KEY_PRESS_ESCAPE:
				setAlive(false);
			case KEY_PRESS_LEFT:
				if (getDirection() != left)
				{
					setDirection(left);
					break;
				}
				else if (hittingBoulder(getX() - 1, getY()) )
					break;
				else if (getX() - 1 >= 0)
					moveTo(getX() - 1, getY());
				else
					moveTo(getX(), getY()); 
				break;
			case KEY_PRESS_RIGHT:
				if (getDirection() != right)
					{
						setDirection(right);
						break;
					}
				else if (hittingBoulder(getX() + 1, getY()))
					break;
				else if (getX() + SPRITE_WIDTH < VIEW_WIDTH)
					moveTo(getX() + 1, getY());
				else
					moveTo(getX(), getY());
				break;
			case KEY_PRESS_UP:
				if (getDirection() != up)
					{
						setDirection(up);
						break;
					}
				else if (hittingBoulder(getX(), getY() + 1))
					break;
				else if (getY() + SPRITE_HEIGHT < VIEW_HEIGHT)
					moveTo(getX(), getY() + 1);
				else
					moveTo(getX(), getY());
				break;
			case KEY_PRESS_DOWN:
				if (getDirection() != down)
					{
						setDirection(down);
						break;
					}
				else if (hittingBoulder(getX(), getY() - 1))
					break;
				else if (getY() - 1 >= 0)
					moveTo(getX(), getY() - 1);
				else
					moveTo(getX(), getY());
				break;
			case KEY_PRESS_TAB:
				if (getNuggets() > 0)
				{
					getWorld()->createActor(new GoldNugget(getWorld(), getX(), getY(), false) );
					setNuggets(getNuggets() - 1);
				}
				break;
			case KEY_PRESS_SPACE:
				if (getWater() > 0)
				{
					setWater(getWater() - 1);
					int x = getX(), y = getY();
					getWorld()->playSound(SOUND_PLAYER_SQUIRT);
					switch (getDirection())
					{
						case up:
							y += 4;
							break;
						case right:
							x += 4;
							break;
						case down:
							y += -4;
							break;
						case left:
							x += -4;
						default:
							break;
					}
					if (getWorld()->isEmptySpace(x, y))
						getWorld()->createActor( new Squirt(getWorld(), x, y, getDirection()) );
				}
				break;
			case 'z':
			case 'Z':
				if (getSonar() > 0)
				{
					setSonar(getSonar() - 1);
					getWorld()->setAllVisible(getX(), getY());
					getWorld()->playSound(SOUND_SONAR);
				}

			default:
				break;
		}
	}
}