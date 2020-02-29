#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
#include <queue>
#include <algorithm>
class Actor;
class Socrates;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* actor);
    bool doProjectileDamage(Actor* projectile, int damage); // if overlapping with a damageable object, damage it, return if damage was done
    void damageSocrates(int damage); // if overlapping Socrates, damage him, return if damage was done
    bool overlapsSocrates(Actor* actor); // determines overlap with Socrates
    void restoreSocratesHealth();
    void refillSocratesFlames();
    void updateGameDetails();
    bool overlap(int x, int y);
    void getRectCoords(int r, int angle, int &x, int &y);
    void getPolarCoords(int x, int y, int &r, int &angle);
    int distanceToSocrates(Actor* a);
    int angleToSocrates(Actor* a);
    int angleToActor(Actor *src, Actor* goal);
    bool dirtBlock(int dir, int dist, Actor* bact);
    int euclideanDistance(Actor* a, Actor* b);
    int euclideanDistance(int ax, int ay, int bx, int by);
    Actor* overlapsFood(Actor* bacterium);
    Actor* findFood(Actor* bacterium);
    
private:
    std::list<Actor*> actors;
    Socrates* socrates;
    std::queue<Actor*> toAdd;
};

#endif // STUDENTWORLD_H_
