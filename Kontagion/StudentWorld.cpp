#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

int euclideanDistance(Actor* a, Actor* b);
int euclideanDistance(int ax, int ay, int bx, int by);

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    // initialize the player
    socrates = new Socrates(this);
    
    // add L pits to petri dish without overlapping
    // make pit objects and check overlap
    for(int p = 0; p < getLevel(); p++){
        bool overlap = true;
        int x = 0, y = 0, radius, angle;
        while(overlap){
            radius = randInt(0, VIEW_RADIUS);
            angle = randInt(0, 359);
            getRectCoords(radius, angle, x, y);
            overlap = this->overlap(x, y);
        }
        actors.push_back(new Pit(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this));
    }
    
    // add min(5*L, 25) food objects to petri dish wihtout overlapping each other or pits
    int numFood = min(5*getLevel(), 25);
    for(int f = 0; f < numFood; f++){
        bool overlap = true;
        int x = 0, y = 0, radius, angle;
        while(overlap){
            radius = randInt(0, VIEW_RADIUS);
            angle = randInt(0, 359);
            getRectCoords(radius, angle, x, y);
            overlap = this->overlap(x, y);
        }
        actors.push_back(new Food(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this));
    }
    
    // initialize all dirt piles without overlapping food or pits
    int numDirtObjs = max(180 - 20 * getLevel(), 20);
    for(int d = 0; d < numDirtObjs; d++){
        bool overlap = true;
        int x = 0, y = 0, pythagx, pythagThreshold;
        while(overlap){
            x = randInt((VIEW_WIDTH/2) - 120, (VIEW_WIDTH/2) + 120);
            pythagx = x - VIEW_WIDTH/2;
            pythagThreshold = sqrt(abs((120*120) - (pythagx*pythagx)));
            y = randInt(VIEW_HEIGHT/2-pythagThreshold, VIEW_HEIGHT/2+pythagThreshold);
            overlap = this->overlap(x, y);
        }
        Actor* curDirt = new Dirt(x, y, this);
        actors.push_back(curDirt);
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // let user doSomething as socrates
    socrates->doSomething();
    
    // stop level if socrates died
    if(socrates->isDead()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    // loop through and let every actor doSomething
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        (*it)->doSomething();
        it++;
        // stop level if socrates died
        if(socrates->isDead()){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    // add all actors waiting to be added to container (i.e. flames/sprays, bacteria)
    while(!toAdd.empty()){
        actors.push_back(toAdd.front());
        toAdd.pop();
    }
    
    bool nextLevel = true;
    // delete dead actors and check if can proceed to next level
    it = actors.begin();
    while(it != actors.end()){
        if((*it)->isDead()){
            delete *it;
            it = actors.erase(it);
        }
        else{
            if((*it)->preventsLevelCompleting())
                nextLevel = false;
            it++;
        }
    }
    if(nextLevel)
        return GWSTATUS_FINISHED_LEVEL;
    
    // add new fungus
    int chanceFungus = max(510 - (getLevel() * 10), 200);
    int randFungus = randInt(0, chanceFungus);
    if(randFungus == 0){
        int x = 0, y = 0, angle;
        angle = randInt(0, 359);
        getRectCoords(VIEW_RADIUS, angle, x, y);
        Fungus* fungus = new Fungus(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this);
        actors.push_back(fungus);
    }
    
    // add new goodies
    int chanceGoodie = max(510 - (getLevel() * 10), 250);
    int randGoodie = randInt(0, chanceGoodie);
    if(randGoodie == 0){
        int typeGoodie = randInt(1, 10);
        int x = 0, y = 0, angle;
        angle = randInt(0, 359);
        getRectCoords(VIEW_RADIUS, angle, x, y);
        Goodie* goodie;
        if(typeGoodie <= 6){
            goodie = new RestoreHealthGoodie(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this);
        }
        else if(typeGoodie <= 9){
            goodie = new FlamethrowerGoodie(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this);
        }
        else{
            goodie = new ExtraLifeGoodie(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y, this);
        }
        actors.push_back(goodie);
    }
    
    // update game text
    updateGameDetails();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete all actors in container
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        delete *it;
        it = actors.erase(it);
    }
    
    // delete player
    delete socrates;
    socrates = nullptr;
}

void StudentWorld::addActor(Actor* actor){
    toAdd.push(actor);
//    actors.push_back(actor);
}

bool StudentWorld::doProjectileDamage(Actor* projectile, int damage){  // not working rn
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        if((*it)->isDamageable() && euclideanDistance(projectile, *it) <= SPRITE_WIDTH){
            (*it)->damage(damage);
            return true;
        }
        else{
            it++;
        }
    }
    return false;
}

void StudentWorld::damageSocrates(int damage){
    socrates->damage(damage);
}

bool StudentWorld::overlapsSocrates(Actor* actor){
    if(euclideanDistance(socrates, actor) < SPRITE_WIDTH){
        return true;
    }
    return false;
}

void StudentWorld::restoreSocratesHealth(){
    socrates->heal();
}

void StudentWorld::refillSocratesFlames(){
    socrates->refillFlames();
}

void StudentWorld::updateGameDetails(){
    ostringstream oss;
    oss << "  Score: ";
    oss.fill('0');
    if(getScore() >= 0)
        oss << setw(6) << getScore();
    else
        oss << '-' << setw(5) << -getScore();
    oss.fill(' ');
    oss << "  Level: " << setw(2) << getLevel()
    << "  Lives: "  << setw(1) << getLives()
    << "  Health: " << setw(2) << socrates->getHP()
    << "  Sprays: " << setw(2) << socrates->getSprays()
    << "  Flames: " << setw(2) << socrates->getFlames();
    setGameStatText(oss.str());
}

bool StudentWorld::overlap(int x, int y){
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        if(euclideanDistance((*it)->getX(), (*it)->getY(), x, y) <= SPRITE_WIDTH){
            return true;
        }
        it++;
    }
    return false;
}

void StudentWorld::getRectCoords(int r, int angle, int &x, int &y){
    if(angle > 359)
        angle %= 360;
    if(angle < 0)
        angle += 360;
    x = r * cos(angle*3.14/180);
    y = r * sin(angle*3.14/180);
}

void StudentWorld::getPolarCoords(int x, int y, int &r, int &angle){
    x = x - VIEW_WIDTH/2;
    y = y - VIEW_HEIGHT/2;
    r = sqrt(x*x + y*y);
    angle = atan2(y, x) * 180 / 3.14;
}

int StudentWorld::distanceToSocrates(Actor* a){
    return euclideanDistance(socrates, a);
}

int StudentWorld::angleToSocrates(Actor *a){
    int x = socrates->getX() - a->getX();
    int y = socrates->getY() - a->getY();
    
    return atan2(y, x) * 180/ 3.14;
}

int StudentWorld::angleToActor(Actor *src, Actor* goal){
    int x = goal->getX() - src->getX();
    int y = goal->getY() - src->getY();
    
    return atan2(y, x) * 180/ 3.14;
}

bool StudentWorld::dirtBlock(int dir, int dist, Actor* bact){
    list<Actor*>::iterator it = actors.begin();
    double x, y;
    bact->getPositionInThisDirection(dir, dist, x, y);
    while(it != actors.end()){
        if(euclideanDistance(x, y, (*it)->getX(), (*it)->getY()) < SPRITE_WIDTH/2){
            if((*it)->blocksBacteriumMovement()){
                return true;
            }
        }
        it++;
    }
    return false;
}

int StudentWorld::euclideanDistance(Actor* a, Actor* b){
    return sqrt(pow(a->getX() - b->getX(), 2) + pow(a->getY() - b->getY(), 2));
}

int StudentWorld::euclideanDistance(int ax, int ay, int bx, int by){
    return sqrt(pow(ax - bx, 2) + pow(ay - by, 2));
}

Actor* StudentWorld::overlapsFood(Actor* bacterium){
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        if((*it)->edible() && euclideanDistance(*it, bacterium) < SPRITE_WIDTH)
            return *it;
        it++;
    }
    return nullptr;
}

Actor* StudentWorld::findFood(Actor* bacterium){
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        if((*it)->edible() && euclideanDistance(bacterium, *it) < 128){
            return *it;
        }
        it++;
    }
    return nullptr;
}
