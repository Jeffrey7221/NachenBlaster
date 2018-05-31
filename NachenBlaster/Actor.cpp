#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

double euclidian_dist(double x1, double y1, double x2, double y2);

/*************************************************************************************
//--------------------------------------- Actor --------------------------------------
**************************************************************************************/

Actor::Actor(const int ID, double startX, double startY, int startDirection, double size, int depth, StudentWorld* setWorld, string setName):
GraphObject(ID, startX, startY, startDirection, size, depth), status(true), theWorld(setWorld), name(setName), offScreen(false) {
}

bool Actor::determineOffScreen() {
    if(getX() < 0 || getX() > VIEW_WIDTH-1) {
        setOffScreen(true);
        return true;
    }
    return false;
}

bool Actor::determineCollision(double otherActorX, double otherActorY, double otherActorR) const {
    if(euclidian_dist(getX(), getY(), otherActorX, otherActorY) < .75*(otherActorR + getRadius()))
        return true;
    else
        return false;
}

bool Actor::determineCollision() const {
    if(euclidian_dist(getX(), getY(), getWorld()->getNachenBlaster()->getX(), getWorld()->getNachenBlaster()->getY()) < .75*(getWorld()->getNachenBlaster()->getRadius() + getRadius()))
        return true;
    else
        return false;
}

bool Actor::isAlien() const {
    return (name == "Smallgon" || name == "Smoregon" || name == "Snagglegon");
}

//        Getters and setters

StudentWorld* Actor::getWorld() const {
    return theWorld;
}

void Actor::setStatus(bool newStatus) {
    status = newStatus;
}

bool Actor::getStatus() {
    return status;
}

void Actor::setOffScreen(bool newOffScreen) {
    offScreen = newOffScreen;
}
bool Actor:: getOffScreen() const {
    return offScreen;
}

string Actor::getName() const {
    return name;
}

double Actor::euclidian_dist(double x1, double y1, double x2, double y2) const {
    double x = x1 - x2;
    double y = y1 - y2;

    double dist;
    dist = pow(x,2) + pow(y, 2);
    dist = sqrt(dist);
    return dist;
}

/*************************************************************************************
//--------------------------------------- SHIPS --------------------------------------
**************************************************************************************/

Ships::Ships(const int ID, double startX, double startY, int startDirection, double size, int depth, StudentWorld* setWorld, string name, double hitpoints):Actor(ID, startX, startY, startDirection, size, depth, setWorld, name), hitPoints(hitpoints)
{ }

void Ships::subtractHitPoints(double hitpoints) {
    hitPoints = hitPoints - hitpoints;
}

double Ships::getHitPoints() const {
    return hitPoints;
}
void Ships::setHitPoints(double setHitPoints) {
    hitPoints = setHitPoints;
}

bool Ships::getStatus() {
    if(getHitPoints() <= 0) {
        setStatus(false);
        getWorld()->addActor(new Explosion(getX(), getY(), getWorld()));
    }
    return Actor::getStatus();
}

/*************************************************************************************
//------------------------------- NachenBlaster --------------------------------------
**************************************************************************************/

NachenBlaster::NachenBlaster(double startX, double startY, StudentWorld* setWorld):Ships(IID_NACHENBLASTER, startX, startY, 0, 1.0, 0, setWorld, "NachenBlaster", 50), cabbageEnergyPoints(30), numFlatTorpedos(0) {
}

void NachenBlaster::doSomething() {
    if(!getStatus()) {
        return;
    }
    int keyValue;
    if(getWorld()->getKey(keyValue)) {
        // user hit a key during this tick!
        switch (keyValue)
        {
            case KEY_PRESS_LEFT:
                if(getX() > 0)
                    moveTo(getX()-6, getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getX() < VIEW_WIDTH)
                    moveTo(getX()+6, getY());
                break;
            case KEY_PRESS_UP:
                if(getY() < VIEW_HEIGHT)
                    moveTo(getX(), getY()+6);
                break;
            case KEY_PRESS_DOWN:
                if(getY() > 0)
                    moveTo(getX(), getY()-6);
                break;
            case KEY_PRESS_SPACE:
                if(cabbageEnergyPoints >= 5) {
                    getWorld()->addActor(new Cabbage(getX()+12, getY(), getWorld()));
                    cabbageEnergyPoints -= 5;
                    getWorld()->playSound(SOUND_PLAYER_SHOOT);
                }
                break;
            case KEY_PRESS_TAB:
                if(numFlatTorpedos > 0) {
                    getWorld()->addActor(new FlatulanTorpedo(getX()+12, getY(), 0, getWorld(), getName()));
                    numFlatTorpedos--;
                    getWorld()->playSound(SOUND_TORPEDO);
                }
                break;
        }
    }
    if(cabbageEnergyPoints < 30)
        cabbageEnergyPoints++;
}

void NachenBlaster::addNumFlatTorpedos(int addFlatTorpedos) {
    numFlatTorpedos += addFlatTorpedos;
}

int NachenBlaster::getNumFlatTorpedos() const {
    return numFlatTorpedos;
}

int NachenBlaster::getCabbages() const {
    return cabbageEnergyPoints;
}

/*************************************************************************************
//--------------------------------- ALIEN --------------------------------------------
*************************************************************************************/

Alien::Alien(const int ID, double startX, double startY,int setFlightPlan, double setFlightSpeed, StudentWorld* setWorld, string setName, double hitpoints)
:Ships(ID, startX, startY, 0, 1.5, 1, setWorld, setName, hitpoints)
{
    flightPlan = setFlightPlan;
    flightSpeed = setFlightSpeed;
    flightPlanLength = 0;
}

void Alien::collidesWithNachen(double hitpoints) {
    getWorld()->getNachenBlaster()->subtractHitPoints(hitpoints);
    setStatus(false);
    getWorld()->addActor(new Explosion(getX(), getY(), getWorld()));
}

bool Alien::checkYBounds() {
    if(getY() >= VIEW_HEIGHT-1) {
        flightPlan = DOWN_LEFT;
        return true;
    } else if(getY() <= 0) {
        flightPlan = UP_LEFT;
        return true;
    }
    return false;
}

void Alien::checkNewFlightPlan() {
    if(flightPlanLength == 0 || checkYBounds()) {
        if(flightPlanLength == 0)
            flightPlan = randInt(1, 3);
        flightPlanLength = randInt(1, 32);
    }
}

void Alien::move() {
    if(flightPlan == UP_LEFT) {
        moveTo(getX() - flightSpeed, getY() + flightSpeed);
    } else if (flightPlan == DOWN_LEFT) {
        moveTo(getX() - flightSpeed, getY() - flightSpeed);
    } else if(flightPlan == LEFT) {
        moveTo(getX()-flightSpeed, getY());
    }
    if(getName() != "Snagglegon")
        flightPlanLength--;
}

bool Alien::tryToShoot() {
    if(getName() == "Snagglegon") {
        if(randInt(1, (15/getWorld()->getLevel())+10) == 1) {
            getWorld()->addActor(new FlatulanTorpedo(getX()-14, getY(), 180, getWorld(), getName()));
            getWorld()->playSound(SOUND_TORPEDO);
            return true;
        }
    }
    else {
        if(randInt(1, (20/getWorld()->getLevel())+5) == 1) {
            getWorld()->addActor(new Turnip(getX()-14, getY(), getWorld()));
            getWorld()->playSound(SOUND_ALIEN_SHOOT);
            return true;
        }
    }
    return false;
}
    
bool Alien::tryToRam() {
    if(randInt(1, (20/getWorld()->getLevel())+5) == 1) {
        flightPlan = LEFT;
        flightPlanLength = VIEW_WIDTH;
        flightSpeed = 5;
        return true;
    }
    return false;
}
/*************************************************************************************
//--------------------------------- Smallgon -----------------------------------------
*************************************************************************************/

Smallgon::Smallgon(double startX, double startY, double setHitPoints, StudentWorld* setWorld)
:Alien(IID_SMALLGON, startX, startY, 0, 2.0, setWorld, "Smallgon", setHitPoints) {
}

void Smallgon::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;

    if(determineCollision()) {
        collidesWithNachen(5.0);
        return;
    }

    checkNewFlightPlan();

    double yDiff = getWorld()->getNachenBlaster()->getY() - getY();
    if(getWorld()->getNachenBlaster()->getX() < getX() && (yDiff <=4) && (yDiff >= -4)) {
        if(tryToShoot())
            return;
    }

    move();
    
    if(determineCollision()) {
        collidesWithNachen(5.0);
        return;
    }
}

/*************************************************************************************
//--------------------------------- Smoregon -----------------------------------------
*************************************************************************************/

Smoregon::Smoregon(double startX, double startY, double setHitPoints, StudentWorld* setWorld)
:Alien(IID_SMOREGON, startX, startY, 0, 2.0, setWorld, "Smoregon", setHitPoints) {
}

void Smoregon::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;

    if(determineCollision()) {
        collidesWithNachen(5.0);
        return;
    }
    
    checkNewFlightPlan();
    
    double yDiff = getWorld()->getNachenBlaster()->getY() - getY();
    
    if(getWorld()->getNachenBlaster()->getX() < getX() && (yDiff <=4 ) && (yDiff >=-4 )) {
        if(tryToShoot() == true)
            return;
        if(tryToRam() == true)
            return;
    }
    
    move();
    
    if(determineCollision()) {
        collidesWithNachen(5.0);
    }
}

/*************************************************************************************
//--------------------------------- Snagglegon ---------------------------------------
*************************************************************************************/

Snagglegon::Snagglegon(double startX, double startY, double setHitPoints, StudentWorld* setWorld)
:Alien(IID_SNAGGLEGON, startX, startY, DOWN_LEFT, 1.75, setWorld, "Snagglegon", setHitPoints) {
}

void Snagglegon::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;

    if(determineCollision()) {
        collidesWithNachen(15.0);
        return;
    }
    
    checkYBounds();
    
    double yDiff = getWorld()->getNachenBlaster()->getY() - getY();
    
    if(getWorld()->getNachenBlaster()->getX() < getX() && (yDiff <=4 ) && (yDiff >=-4 )) {
        if(tryToShoot())
            return;
    }
    
    move();
    
    if(determineCollision()) {
        collidesWithNachen(15.0);
    }
}

/************************************************************************************
//--------------------------------- STAR --------------------------------------------
*************************************************************************************/

Star::Star(double startX, double startY, double size, StudentWorld* setWorld)
:Actor(IID_STAR, startX, startY, 0, size, 3, setWorld, "Star") {
}

void Star::doSomething() {
    moveTo(getX()-1, getY());
    determineOffScreen();
}
void Star::subtractHitPoints(double hitpoints){};

/*************************************************************************************
//------------------------------- EXPLOSION ------------------------------------------
*************************************************************************************/

Explosion::Explosion(double startX, double startY, StudentWorld* setWorld)
:Actor(IID_EXPLOSION, startX, startY, 0, 1.0, 0, setWorld, "Explosion"), lifeSpan(4){
}

void Explosion::doSomething() {
    setSize(getSize()*1.5);
    lifeSpan--;
    if(lifeSpan == 0)
        setOffScreen(true);
}
void Explosion::subtractHitPoints(double hitpoints){};

/*************************************************************************************
//------------------------------------ Projectile ------------------------------------
**************************************************************************************/

Projectile::Projectile(const int ID, double startX, double startY, int setDirection, StudentWorld* setWorld, string setName)
:Actor(ID, startX, startY, setDirection, 0.5, 1, setWorld, setName)
{ }

void Projectile::collidesWith(double hitpoints, Actor *collidingActor) {
    collidingActor->subtractHitPoints(2);
    if(collidingActor->getStatus())
        getWorld()->playSound(SOUND_BLAST);
    setStatus(false);
}

/*************************************************************************************
//---------------------------------- Cabbage -----------------------------------------
*************************************************************************************/

Cabbage::Cabbage(double startX, double startY,StudentWorld* setWorld)
:Projectile(IID_CABBAGE, startX, startY, 0, setWorld, "Cabbage")
{ }

void Cabbage::subtractHitPoints(double hitpoints) {}

void Cabbage::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;
    Actor* collidingAlien = getWorld()->getOneCollidingAlien(this);
    if(collidingAlien != nullptr) {
        collidesWith(2, collidingAlien);
        return;
    }
    moveTo(getX()+8, getY());
    setDirection(getDirection()+20);
    collidingAlien = getWorld()->getOneCollidingAlien(this);
    if(collidingAlien != nullptr) {
        collidesWith(2, collidingAlien);
        return;
    }
}

/*************************************************************************************
//----------------------------------- Turnip -----------------------------------------
*************************************************************************************/

Turnip::Turnip(double startX, double startY, StudentWorld* setWorld)
:Projectile(IID_TURNIP, startX, startY, 0, setWorld, "Turnip")
{ }

void Turnip::subtractHitPoints(double hitpoints) {}

void Turnip::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;
    if(determineCollision()) {
        collidesWith(2, getWorld()->getNachenBlaster());
        return;
    }
    moveTo(getX()-6, getY());
    setDirection(getDirection()+20);
    if(determineCollision()) {
        collidesWith(2, getWorld()->getNachenBlaster());
        return;
    }
}

/*************************************************************************************
//------------------------------ FlatulanTorpedo -------------------------------------
*************************************************************************************/

FlatulanTorpedo::FlatulanTorpedo(double startX, double startY, int setDirection, StudentWorld* setWorld, string setFiredBy)
:Projectile(IID_TORPEDO, startX, startY, setDirection, setWorld, "Turnip"), firedBy(setFiredBy)
{ }

void FlatulanTorpedo::subtractHitPoints(double hitpoints) {}

void FlatulanTorpedo::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;
    if(firedBy == "Snagglegon") {
        if(determineCollision()) {
            collidesWith(8, getWorld()->getNachenBlaster());
            return;
        }
        moveTo(getX()-8, getY());
        if(determineCollision()) {
            collidesWith(8, getWorld()->getNachenBlaster());
            return;
        }
    }
    else if(firedBy == "NachenBlaster") {
        Actor* collidingAlien = getWorld()->getOneCollidingAlien(this);
        if(collidingAlien != nullptr) {
            collidesWith(8, collidingAlien);
            return;
        }
        moveTo(getX()+8, getY());
        collidingAlien = getWorld()->getOneCollidingAlien(this);
        if(collidingAlien != nullptr) {
            collidesWith(8, collidingAlien);
            return;
        }
    }
}

/*************************************************************************************
//-------------------------------------- Goodies -------------------------------------
**************************************************************************************/
Goodies::Goodies(const int ID, double startX, double startY, StudentWorld* setStudentWorld, string setName)
:Actor(ID, startX, startY, 0, 0.5, 1, setStudentWorld, setName)
{  }

void Goodies::subtractHitPoints(double hitpoints) {}

void Goodies::doesCollide() {
    getWorld()->increaseScore(100);
    getWorld()->playSound(SOUND_GOODIE);
    setStatus(false);
}

void Goodies::doSomething() {
    if(!getStatus() || determineOffScreen())
        return;
    if(getY() < 0) {
        setOffScreen(true);
        return;
    }
    if(determineCollision()) {
        doesCollide();
        doDifferentStuff();
        return;
    }
    moveTo(getX()-.75, getY()-.75);
    if(determineCollision()) {
        doesCollide();
        doDifferentStuff();
        return;
    }
}

/*************************************************************************************
 //------------------------------- ExtraLifeGoodie ---------------------------------------
 *************************************************************************************/
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld* setStudentWorld)
:Goodies(IID_LIFE_GOODIE, startX, startY, setStudentWorld, "ExtraLifeGoodie")
{ }

void ExtraLifeGoodie::doDifferentStuff() {
        getWorld()->incLives();
}

/*************************************************************************************
//------------------------------- RepairGoodie ---------------------------------------
*************************************************************************************/

RepairGoodie::RepairGoodie(double startX, double startY, StudentWorld* setStudentWorld)
:Goodies(IID_REPAIR_GOODIE, startX, startY, setStudentWorld, "RepairGoodie")
{ }

void RepairGoodie::doDifferentStuff() {
    int NachBlastHP = getWorld()->getNachenBlaster()->getHitPoints();
    if(NachBlastHP > 40)
        getWorld()->getNachenBlaster()->setHitPoints(50);
    else
        getWorld()->getNachenBlaster()->setHitPoints(NachBlastHP + 10);
}

/*************************************************************************************
//--------------------------- FlatulanTorpedoGoodie ----------------------------------
*************************************************************************************/

FlatulanTorpedoGoodie::FlatulanTorpedoGoodie(double startX, double startY, StudentWorld* setStudentWorld)
:Goodies(IID_TORPEDO_GOODIE, startX, startY, setStudentWorld, "FlatulanTorpedoGoodie")
{ }

void FlatulanTorpedoGoodie::doDifferentStuff() {
    getWorld()->getNachenBlaster()->addNumFlatTorpedos(5);
}

