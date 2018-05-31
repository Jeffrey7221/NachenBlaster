#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

////----------------- End Helper functions------------------------------


GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir), shipsDestroyed(0), shipsOnScreen(0), level(1)
{ }

StudentWorld::~StudentWorld() {
    cleanUp();
}

NachenBlaster* StudentWorld::getNachenBlaster() {
    return theNachenBlaster;
}

Actor* StudentWorld::getOneCollidingAlien(const Actor* a) const {
    list<Actor*>::const_iterator it = actors.begin();
    while(it != actors.end()) {
        if((*it)->isAlien()) {
            if(a->determineCollision((*it)->getX(), (*it)->getY(), (*it)->getRadius())) {
                return (*it);
            }
        }
        it++;
    }
    return nullptr;
}

int StudentWorld::getLevel() {
    return level;
}

/*The init() method is responsible for constructing a representation of the current level in
 your StudentWorld object and populating it with initial objects (e.g., stars), using one or
 more data structures that you come up with. */
int StudentWorld::init()
{
    shipsDestroyed = 0;
    shipsOnScreen = 0;
    theNachenBlaster = new NachenBlaster(0, 128, this);
    for(int i = 0; i < 30; i++) {
        actors.push_back(new Star(randInt(0, VIEW_WIDTH-1), randInt(0, VIEW_HEIGHT-1), randInt(5, 50)/100.0, this));
    }
    return GWSTATUS_CONTINUE_GAME;
}

/*Each time the move() method is called, it must run a single tick of the game. This
 means that it is responsible for asking each of the game actors (e.g., the NachenBlaster,
 each Smallgon, goodie, projectile, star, explosion, etc.) to try to do something: e.g., move
 themselves and/or perform their specified behavior. Finally, this method is responsible
 for disposing of (i.e., deleting) actors that need to disappear during a given tick (e.g., a
 cabbage that flies off the screen or collides with an enemy, a dead Smallgon, etc.).*/

int StudentWorld::move()
{
    //Allow all actors to do something
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()) {
        if((*it)->getStatus()) {
            (*it)->doSomething();
            if(!theNachenBlaster->getStatus()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if(shipsDestroyed == (6+4*level)) {
                level++;
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++;
    }
    theNachenBlaster->doSomething();
    removeDeadActors();
    spawnNewActors();
    updateGameStatText();
    return GWSTATUS_CONTINUE_GAME;
}

/*The cleanup() method is responsible for freeing all actors (e.g., all
 Smallgon objects, all star objects, all explosion objects, all projectiles, all goodie objects,
 the NachenBlaster object, etc.) that are currently in the game. This includes all actors
 created during either the init() method or introduced during subsequent game play by the
 actors in the game (e.g., a turnip that was added to the screen by a Smoregon) that have
 not yet been removed from the game. */

void StudentWorld::cleanUp()
{
    list<Actor*>:: iterator it = actors.begin();
    while(it != actors.end()) {
        delete *it;
        it = actors.erase(it);
    }
    if(theNachenBlaster != NULL) {
        delete theNachenBlaster;
        theNachenBlaster = NULL;
    }
}

void StudentWorld::spawnNewActors() {
    if(randInt(1, 15) == 1) {
        actors.push_back(new Star(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), randInt(5, 50)/100.0, this));
    }
    int remainingShips = (6+4*level) - shipsDestroyed;
    int maxShips = 4 + .5*level;
    if(shipsOnScreen < min(maxShips, remainingShips)) {
        int maxProb = 60 + 20 +level*5 + 5 + level*10;
        int shipSpawn = randInt(1, maxProb);
        if(shipSpawn <= 60) {
            actors.push_back(new Smallgon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 5*(1+(level-1)*.1), this));
        } else if (shipSpawn <= (60 + 20 + level*5)) {
            actors.push_back(new Smoregon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 5*(1+(level-1)*.1), this));
        } else {
            actors.push_back(new Snagglegon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 10*(1+(level-1)*.1), this));
        }
        shipsOnScreen++;
    }
}

//Maybe merge these two together
void StudentWorld::removeDeadActors() {
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()) {
        if((*it)->getStatus() == false || (*it)->getOffScreen() == true) {
            if((*it)->getStatus() == false && ((*it)->isAlien())) {
                playSound(SOUND_DEATH);
                if((*it)->getName() == "Smoregon") {
                    if(randInt(1, 3) == 1) {
                        if(randInt(1, 2) == 1) {
                            actors.push_back(new RepairGoodie((*it)->getX(), (*it)->getY(), this));
                        } else {
                            actors.push_back(new FlatulanTorpedoGoodie((*it)->getX(), (*it)->getY(), this));
                        }
                    }
                    increaseScore(250);
                }
                else if((*it)->getName() == "Smallgon") {
                    increaseScore(250);
                }
                else {
                    if(randInt(1, 6) == 1)
                        actors.push_back(new ExtraLifeGoodie((*it)->getX(), (*it)->getY(), this));
                    increaseScore(1000);
                }
                shipsDestroyed++;
                shipsOnScreen--;
            }
            if((*it)->getOffScreen() && (*it)->isAlien()) {
                shipsOnScreen--;
            }
            delete *it;
            it = actors.erase(it);
        }
        else {
            it++;
        }
    }
}

void StudentWorld::updateGameStatText() {
    ostringstream oss;
    oss.setf(ios::fixed);
    oss.precision(0);
    oss << "Lives:";
    oss << setw(2) << getLives();
    oss << "  Health:";
    oss << setw(4) << 100*(theNachenBlaster->getHitPoints()/50) << "%";
    oss << "  Score: ";
    oss << setw(1) << getScore();
    oss << "  Level:";
    oss << setw(2) << level;
    oss << "  Cabbages:";
    oss << setw(4) << 100*(theNachenBlaster->getCabbages()/30.0) <<"%";
    oss << "  Torpedos:";
    oss << setw(2) << theNachenBlaster->getNumFlatTorpedos();
    string s = oss.str();
    setGameStatText(s);
}

void StudentWorld::addActor(Actor* a) {
    if(a->getName() == "Explosion")
        actors.push_front(a);
    else
        actors.push_back(a);
}

//----------------- Helper functions------------------------------------
