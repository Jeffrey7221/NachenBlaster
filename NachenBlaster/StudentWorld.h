#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

//------
#include <list>
#include "Actor.h"
using namespace std;
//------

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor; class NachenBlaster;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    NachenBlaster* getNachenBlaster();
    Actor* getOneCollidingAlien(const Actor* a) const;
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void spawnNewActors();
    void removeDeadActors();
    void addActor(Actor* a);
    void updateGameStatText();
    int getLevel();
private:
    NachenBlaster* theNachenBlaster;
    list<Actor*> actors;
    int shipsDestroyed;
    int shipsOnScreen;
    int level;
};

//----------------- Helper functions------------------------------------



#endif // STUDENTWORLD_H_

