#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//GraphObject(int imageID, int startX, int startY, int startDirection, double size, int depth)
class Actor: public GraphObject
{
public:
    Actor(const int ID, double startX, double startY, int startDirection, double size, int depth, StudentWorld* setWorld, string setName);
    bool determineOffScreen();
    bool determineCollision(double otherActorX, double otherActorY, double otherActorR) const;
    bool determineCollision() const;
    bool isAlien() const;
    // Getters and setters
    StudentWorld* getWorld() const;
    virtual bool getStatus();
    bool getOffScreen() const;
    void setStatus(bool newStatus);
    string getName() const;
    void setOffScreen(bool newOffScreen);
    virtual void doSomething() = 0;
    virtual void subtractHitPoints(double hitpoints) = 0;
private:
    double euclidian_dist(double x1, double y1, double x2, double y2) const;
    StudentWorld* theWorld;
    bool status;
    bool offScreen;
    string name;
};

//---------------------------------- SHIPS -----------------------------------------
class Ships: public Actor {
public:
    Ships(const int ID, double startX, double startY, int startDirection, double size, int depth, StudentWorld* setWorld, string name, double hitpoints);
    virtual void subtractHitPoints(double hitpoints);
    virtual bool getStatus();
    double getHitPoints() const;
    void setHitPoints(double setHitPoints);
private:
    double hitPoints;
};

//-------------------- NACHEN BLASTER -----------------------------------
class NachenBlaster: public Ships
{
public:
    NachenBlaster(double startX, double startY, StudentWorld* setWorld);
    virtual void doSomething();
    void addNumFlatTorpedos(int addFlatTorpedos);
    int getNumFlatTorpedos() const;
    int getCabbages() const;
private:
    int cabbageEnergyPoints;
    int numFlatTorpedos;
};

//------------------------------------------ ALIEN --------------------------------------------
const int LEFT  = 1;
const int UP_LEFT = 2;
const int DOWN_LEFT = 3;

class Alien: public Ships
{
public:
    Alien(const int ID, double startX, double startY, int setFlightPlan, double setFlightSpeed, StudentWorld* setWorld, string setName, double hitpoints);
    void collidesWithNachen(double hitpoints);
    bool checkYBounds();
    void checkNewFlightPlan();
    void move();
    bool tryToShoot();
    bool tryToRam();
private:
    int flightPlan;
    double flightSpeed;
    int flightPlanLength;
};

//-------------------- Smallgon --------------------------------------------
class Smallgon: public Alien
{
public:
    Smallgon(double startX, double startY, double setHitPoints, StudentWorld* setWorld);
    virtual void doSomething();
};

//-------------------- Smoregon --------------------------------------------
class Smoregon: public Alien
{
public:
    Smoregon(double startX, double startY, double setHitPoints, StudentWorld* setWorld);
    virtual void doSomething();
};

//-------------------- Snagglegon --------------------------------------------
class Snagglegon: public Alien
{
public:
    Snagglegon(double startX, double startY, double setHitPoints, StudentWorld* setWorld);
    virtual void doSomething();
};

//-------------------- STAR ---------------------------------------------
class Star: public Actor
{
public:
    Star(double startX, double startY, double size, StudentWorld* setWorld);
    virtual void doSomething();
private:
    virtual void subtractHitPoints(double hitpoints);
};

//-------------------- EXPLOSION ---------------------------------------
class Explosion: public Actor
{
public:
    Explosion(double startX, double startY, StudentWorld* setWorld);
    virtual void doSomething();
private:
    virtual void subtractHitPoints(double hitpoints);
private:
    int lifeSpan;
};

//-------------------- PROJECTILE ---------------------------------------
class Projectile: public Actor
{
public:
    Projectile(const int ID, double startX, double startY, int setDirection, StudentWorld* setWorld, string name);
    void collidesWith(double hitpoints, Actor* ship);
};

class Cabbage: public Projectile
{
public:
    Cabbage(double startX, double startY, StudentWorld* setWorld);
    virtual void doSomething();
private:
    virtual void subtractHitPoints(double hitpoints);
};

class Turnip: public Projectile
{
public:
    Turnip(double startX, double startY, StudentWorld* setWorld);
    virtual void doSomething();
    virtual void subtractHitPoints(double hitpoints);
};

class FlatulanTorpedo: public Projectile
{
public:
    FlatulanTorpedo(double startX, double startY, int setDirection, StudentWorld* setWorld, string setFiredBy);
    virtual void doSomething();
    virtual void subtractHitPoints(double hitpoints);
private:
    string firedBy;
};

//-------------------- GOODIES ------------------------------------------
class Goodies: public Actor
{
public:
    Goodies(const int ID, double startX, double startY, StudentWorld* setStudentWorld, string setName);
    virtual void doSomething();
    virtual void doesCollide();
private:
    virtual void subtractHitPoints(double hitpoints);
    virtual void doDifferentStuff() = 0;
};

class ExtraLifeGoodie: public Goodies
{
public:
    ExtraLifeGoodie(double startX, double startY, StudentWorld* setStudentWorld);
    virtual void doDifferentStuff();
};

class RepairGoodie: public Goodies
{
public:
    RepairGoodie(double startX, double startY, StudentWorld* setStudentWorld);
    virtual void doDifferentStuff();
};

class FlatulanTorpedoGoodie: public Goodies
{
public:
    FlatulanTorpedoGoodie(double startX, double startY, StudentWorld* setStudentWorld);
    virtual void doDifferentStuff();
};

#endif // ACTOR_H_

