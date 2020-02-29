#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
void getRectCoords(int r, int angle, int &x, int &y);

class Actor : public GraphObject{
public:
    Actor(int IID, double startX, double startY, int startDir, int depth, StudentWorld* sw, bool damageable);
    virtual ~Actor();
    virtual void doSomething();
    void die();
    bool isDead();
    bool isDamageable();
    virtual void damage(int harm);
    StudentWorld* getMyStudentWorld();
    virtual bool preventsLevelCompleting();
    virtual bool blocksBacteriumMovement() const;
    virtual bool edible();
    
private:
    bool m_alive;
    StudentWorld* m_studentWorld;
    bool m_damageable;
};

class Dirt : public Actor{
public:
    Dirt(int startX, int startY, StudentWorld* sw);
    virtual ~Dirt();
    virtual void doSomething();
    virtual bool blocksBacteriumMovement() const;
};

class Pit : public Actor{
public:
    Pit(int startX, int startY, StudentWorld* sw);
    virtual ~Pit();
    virtual void doSomething(); // check if out of bacteria and die(), else spawnBacti() with 1/50th chance every tick
    virtual bool preventsLevelCompleting();
private:
    void spawnBacti(); // pick randInt(1,3) and add new bacteria to *sw->actors based on RNG, decrement inventory of that bacteria, and play SOUND_BACTERIUM_BORN
    int m_numRegSalmonella;
    int m_numAgroSalmonella;
    int m_numEColi;
};

class Food : public Actor{
public:
    Food(int startX, int startY, StudentWorld* sw);
    virtual ~Food();
    virtual void doSomething();
    virtual bool edible();
};

class Projectile : public Actor{
public:
    Projectile(int IID, int startX, int startY, int startDir, StudentWorld* sw, int travelDistance, int dP);
    virtual ~Projectile();
    void doSomething();
private:
    int m_travelDistance;
    int m_damagePoints;
};

class Flame : public Projectile{
public:
    Flame(int startX, int startY, int startDir, StudentWorld* sw);
    virtual ~Flame();
};

class Spray : public Projectile{
public:
    Spray(int startX, int startY, int startDir, StudentWorld* sw);
    virtual ~Spray();
};

class Goodie : public Actor{
public:
    Goodie(int IID, int startX, int startY, StudentWorld* sw);
    virtual ~Goodie();
    virtual void doSomething();
    virtual void doGoodieTask() = 0;
private:
    int m_lifetime;
};

class RestoreHealthGoodie : public Goodie{
public:
    RestoreHealthGoodie(int startX, int startY, StudentWorld* sw);
    virtual ~RestoreHealthGoodie();
    virtual void doGoodieTask();
};

class FlamethrowerGoodie : public Goodie{
public:
    FlamethrowerGoodie(int startX, int startY, StudentWorld* sw);
    virtual ~FlamethrowerGoodie();
    virtual void doGoodieTask();
};

class ExtraLifeGoodie : public Goodie{
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* sw);
    virtual ~ExtraLifeGoodie();
    virtual void doGoodieTask();
};

class Fungus : public Goodie{
public:
    Fungus(int startX, int startY, StudentWorld* sw);
    virtual ~Fungus();
    virtual void doGoodieTask();
};



class Agent : public Actor{
public:
    Agent(int IID, double startX, double startY, int startDir, int depth, StudentWorld* sw, int health, bool damageable);
    virtual ~Agent();
    virtual void doSomething();
    int getHP();   // accessor method for m_hp
    virtual void damage(int harm);  // mutator method for m_hp
    virtual void playDamageSound(); // plays dead or hurt sound for player and bacteria
private:
    int m_hp;
};

class Socrates : public Agent{
public:
    Socrates(StudentWorld* sw);
    virtual ~Socrates();
    virtual void doSomething();
    void heal();
    void refillFlames();
    int getFlames();
    int getSprays();
    virtual void playDamageSound();
    
private:
    int m_sprayCharges;
    int m_flameCharges;
};

class Bacterium : public Agent{
public:
    Bacterium(int IID, double startX, double startY, StudentWorld* sw, int health, int socratesDamage);
    virtual ~Bacterium();
    virtual void doSomething();
    virtual bool preventsLevelCompleting();
    virtual void damage(int harm);
    virtual void doSpecializedMovement();
    virtual bool doAggressiveThing();
    int getMPD();
    void setMPD(int mpd);
    virtual void divide(int newx, int newy, StudentWorld* sw);
private:
    int m_foodEatenCount;
    int m_movementPlanDistance;
    int m_socratesDamage;
};

class EColi : public Bacterium{
public:
    EColi(double startX, double startY, StudentWorld* sw);
    virtual ~EColi();
    virtual void playDamageSound();
    virtual void doSpecializedMovement();
    virtual void divide(int newx, int newy, StudentWorld* sw);
};

class Salmonella : public Bacterium{
public:
    Salmonella(double startX, double startY, StudentWorld* sw, int hp, int socratesDamage);
    virtual ~Salmonella();
    virtual void playDamageSound();
    virtual void doSpecializedMovement();
};

class RegularSalmonella : public Salmonella{
public:
    RegularSalmonella(double startX, double startY, StudentWorld* sw);
    virtual ~RegularSalmonella();
    virtual void divide(int newx, int newy, StudentWorld* sw);
};

class AggressiveSalmonella : public Salmonella{
public:
    AggressiveSalmonella(double startX, double startY, StudentWorld* sw);
    virtual ~AggressiveSalmonella();
    virtual bool doAggressiveThing();
     virtual void divide(int newx, int newy, StudentWorld* sw);
};

#endif // ACTOR_H_
