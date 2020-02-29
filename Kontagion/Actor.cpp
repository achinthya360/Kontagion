#include "Actor.h"
#include <algorithm>
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
const bool ALIVE = true;
const bool DEAD = false;

// Actor : GraphObject
Actor::Actor(int IID, double startX, double startY, int startDir, int depth, StudentWorld* sw, bool damageable)
: GraphObject(IID, startX, startY, startDir, depth)
{
    m_alive = ALIVE;
    m_studentWorld = sw;
    m_damageable = damageable;
}

Actor::~Actor(){}

void Actor::doSomething(){
    return;
}

void Actor::die(){
    m_alive = DEAD;
}

bool Actor::isDead(){
    return !m_alive;
}

bool Actor::isDamageable(){
    return m_damageable;
}

void Actor::damage(int harm){
    die();
}

StudentWorld* Actor::getMyStudentWorld(){
    return m_studentWorld;
}

bool Actor::preventsLevelCompleting(){
    return false;
}

bool Actor::blocksBacteriumMovement() const{
    return false;
}

bool Actor::edible(){
    return false;
}

// Dirt : Actor
Dirt::Dirt(int startX, int startY, StudentWorld* sw)
: Actor(IID_DIRT, startX, startY, 0, 1, sw, true){}

Dirt::~Dirt(){}

void Dirt::doSomething(){
    return;
}

bool Dirt::blocksBacteriumMovement() const{
    return true;
}

// Pit : Actor
Pit::Pit(int startX, int startY, StudentWorld* sw)
: Actor(IID_PIT, startX, startY, 0, 1, sw, false)
{
    m_numEColi = 2;
    m_numRegSalmonella = 5;
    m_numAgroSalmonella = 3;
}

Pit::~Pit(){}

void Pit::doSomething(){ // check if out of bacteria and die(), else spawnBacti() with 1/50th chance every tick
    if(m_numEColi == 0 && m_numRegSalmonella == 0 && m_numAgroSalmonella == 0)
        //emitted all bacteria
        die();
    else{
        // 1/50th spawning chance
        if(randInt(1, 50) == 1)
            spawnBacti();
    }
}

void Pit::spawnBacti(){ // pick randInt(1,3) and add new bacteria to *sw->actors based on RNG, decrement inventory of that bacteria, and play SOUND_BACTERIUM_BORN
    StudentWorld* mySW = getMyStudentWorld();
    Bacterium* newBacti = nullptr;
    while(!newBacti){
        int bactType = randInt(0, 2);
        switch(bactType){
            case 0:
                if(m_numEColi == 0)
                    break;
                else{
                    newBacti = new EColi(getX(), getY(), mySW);
                    m_numEColi--;
                }
                break;
            case 1:
                if(m_numAgroSalmonella == 0)
                    break;
                else{
                    newBacti = new AggressiveSalmonella(getX(), getY(), mySW);
                    m_numAgroSalmonella--;
                }
                break;
            case 2:
                if(m_numRegSalmonella == 0)
                    break;
                else{
                    newBacti = new RegularSalmonella(getX(), getY(), mySW);
                    m_numRegSalmonella--;
                }
                break;
            default:
                break;
        }
    }
    mySW->addActor(newBacti);
    mySW->playSound(SOUND_BACTERIUM_BORN);
}

bool Pit::preventsLevelCompleting(){
    return true;
}


// Food : Actor
Food::Food(int startX, int startY, StudentWorld* sw)
: Actor(IID_FOOD, startX, startY, 90, 1, sw, false){}

Food::~Food(){}

void Food::doSomething(){
    return;
}

bool Food::edible(){
    return true;
}


// Projectile : Actor
Projectile::Projectile(int IID, int startX, int startY, int startDir, StudentWorld* sw, int travelDistance, int dP)
: Actor(IID, startX, startY, startDir, 1, sw, false){
    m_travelDistance = travelDistance;
    m_damagePoints = dP;
}

Projectile::~Projectile(){}

void Projectile::doSomething(){
    if(isDead())
        return;
    // check if overlapping with a damageable object (use isDamageable() function)
    // if overlapping any, damage one of the overlapping objects
    bool damageDone = getMyStudentWorld()->doProjectileDamage(this, m_damagePoints);
    if(damageDone){
        die();  // dissipates after damaging an object
        return;
    }
    else{
        moveAngle(getDirection(), SPRITE_WIDTH);
        m_travelDistance -= SPRITE_WIDTH;
        if(m_travelDistance < 0)
            die(); // dissipates after completing its maximum travel distance
    }
}


// Flame : Projectile
Flame::Flame(int startX, int startY, int startDir, StudentWorld* sw)
: Projectile(IID_FLAME, startX, startY, startDir, sw, 32, 5){}

Flame::~Flame(){}

//void Flame::doSomething(){}


// Spray : Projectile
Spray::Spray(int startX, int startY, int startDir, StudentWorld* sw)
: Projectile(IID_SPRAY, startX, startY, startDir, sw, 112, 2){}

Spray::~Spray(){}

//void Spray::doSomething(){}


// Goodie : Actor
Goodie::Goodie(int IID, int startX, int startY, StudentWorld* sw)
: Actor(IID, startX, startY, 0, 1, sw, true)
{
    m_lifetime = std::max(randInt(0, 300-10*getMyStudentWorld()->getLevel() - 1), 50);
}

Goodie::~Goodie(){}

void Goodie::doSomething(){
    if(isDead())
        return;
    if (getMyStudentWorld()->overlapsSocrates(this)) {
        doGoodieTask();
        die();
        return;
    }
    m_lifetime--;
    if(m_lifetime <= 0)
        die();
}

// RestoreHealthGoodie : Goodie
RestoreHealthGoodie::RestoreHealthGoodie(int startX, int startY, StudentWorld* sw)
: Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, sw){}

RestoreHealthGoodie::~RestoreHealthGoodie(){}

void RestoreHealthGoodie::doGoodieTask(){
    StudentWorld* sw = getMyStudentWorld();
    sw->restoreSocratesHealth();
    sw->increaseScore(250);
}


// FlamethrowerGoodie : Goodie
FlamethrowerGoodie::FlamethrowerGoodie(int startX, int startY, StudentWorld* sw)
: Goodie(IID_FLAME_THROWER_GOODIE, startX, startY, sw){}

FlamethrowerGoodie::~FlamethrowerGoodie(){}

void FlamethrowerGoodie::doGoodieTask(){
    StudentWorld* sw = getMyStudentWorld();
    sw->refillSocratesFlames();
    sw->increaseScore(300);
}


// ExtraLifeGoodie
ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* sw)
: Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, sw){}

ExtraLifeGoodie::~ExtraLifeGoodie(){}

void ExtraLifeGoodie::doGoodieTask(){
    StudentWorld* sw = getMyStudentWorld();
    sw->incLives();
    sw->increaseScore(500);
}


// Fungus : Goodie
Fungus::Fungus(int startX, int startY, StudentWorld* sw)
: Goodie(IID_FUNGUS, startX, startY, sw){}

Fungus::~Fungus(){}

void Fungus::doGoodieTask(){
    StudentWorld* sw = getMyStudentWorld();
    sw->damageSocrates(20);
    sw->increaseScore(-50);
}


// Agent : Actor
Agent::Agent(int IID, double startX, double startY, int startDir, int depth, StudentWorld* sw, int health, bool damageable)
:Actor(IID, startX, startY, startDir, depth, sw, damageable){
    m_hp = health;
}

Agent::~Agent(){}

void Agent::doSomething(){
    if (m_hp < 1)
        die();
}

int Agent::getHP(){ // accessor method for m_hp
    return m_hp;
}

void Agent::damage(int harm){ // mutator method for m_hp
    m_hp -= harm;
    if(m_hp <= 0){
        die();
    }
    playDamageSound();
}

void Agent::playDamageSound(){
    return;
}


// Socrates : Actor
Socrates::Socrates(StudentWorld* sw)
: Agent(IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0, sw, /*hp == */100, false){
    m_sprayCharges = 20;
    m_flameCharges = 5;
}

Socrates::~Socrates(){}

void Socrates::doSomething(){
    if(getHP() <= 0){   // player died
        die();
        return;
    }
    else{
        // if player pressed a key
        StudentWorld* mySW = getMyStudentWorld();
        int keyInput = KEY_PRESS_RIGHT;
        if(mySW->getKey(keyInput))
        {
            switch (keyInput)
            {
                case KEY_PRESS_LEFT:
                {
                    int x, y;
                    mySW->getRectCoords(VIEW_RADIUS, getDirection() - 175, x, y);
                    moveTo(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y);
                    setDirection(getDirection() + 5);
                    break;
                }
                case KEY_PRESS_RIGHT:
                {
                    int x, y;
                    mySW->getRectCoords(VIEW_RADIUS, getDirection() + 175, x, y);
                    moveTo(VIEW_WIDTH/2 + x, VIEW_HEIGHT/2 + y);
                    setDirection(getDirection() - 5);
                    break;
                }
                case KEY_PRESS_SPACE:
                {
                    if(m_sprayCharges > 0){
                        double x, y;
                        this->getPositionInThisDirection(getDirection(), SPRITE_WIDTH, x, y);
                        Spray* spray = new Spray(x, y, getDirection(), mySW);
                        mySW->addActor(spray);
                        mySW->playSound(SOUND_PLAYER_SPRAY);
                        m_sprayCharges--;
                    }
                    break;
                }
                case KEY_PRESS_ENTER:
                    if(m_flameCharges > 0){
                        int dir = getDirection();
                        double x, y;
                        for(int numFlames = 0; numFlames < 16; dir += 22, numFlames++){
                            this->getPositionInThisDirection(dir, SPRITE_WIDTH, x, y);
                            Flame* flame = new Flame(x, y, dir, mySW);
                            mySW->addActor(flame);
                        }
                        m_flameCharges--;
                        mySW->playSound(SOUND_PLAYER_FIRE);
                    }
                default:
                    break;
            }
        }
        // if player didn't press a key
        else{
            if(m_sprayCharges < 20){
                m_sprayCharges++;
            }
        }
    }
}

void Socrates::heal(){
    damage(getHP()-100);  // does negative damage aka healing
}

void Socrates::refillFlames(){
    m_flameCharges += 5;
}

int Socrates::getFlames(){
    return m_flameCharges;
}

int Socrates::getSprays(){
    return m_sprayCharges;
}

void Socrates::playDamageSound(){
    if(isDead())
        getMyStudentWorld()->playSound(SOUND_PLAYER_DIE);
    else
        getMyStudentWorld()->playSound(SOUND_PLAYER_HURT);
}

// Bacterium : Actor
Bacterium::Bacterium(int IID, double startX, double startY, StudentWorld* sw, int health, int socratesDamage)
:Agent(IID, startX, startY, 90, 0, sw, health, true){
    m_socratesDamage = socratesDamage;
    m_movementPlanDistance = 0;
}

Bacterium::~Bacterium(){}

void Bacterium::doSomething(){
    Agent::doSomething();
    // Step 1: check if alive
    if(isDead())
        return;
    
    // do aggressive movement if aggressive salmonella
    bool aggressionComplete = false;
    aggressionComplete = doAggressiveThing();
    
    // Step 2: check if overlaps with Socrates and do damage
    StudentWorld* mySW = getMyStudentWorld();
    if(mySW->overlapsSocrates(this)){
        mySW->damageSocrates(m_socratesDamage);
    }
    
    // Step 3: check if enough food eaten to divide
    else{
        if(m_foodEatenCount == 3){
            // compute new x coordinate for copy
            int curx = getX();
            int newx = curx;
            if(curx < VIEW_WIDTH/2)
                newx += SPRITE_WIDTH/2;
            else if(curx > VIEW_WIDTH/2)
                newx -= SPRITE_WIDTH/2;
            
            // compute new y coordinate for copy
            int cury = getY();
            int newy = cury;
            if(cury < VIEW_WIDTH/2)
                newy += SPRITE_WIDTH/2;
            else if(cury > VIEW_WIDTH/2)
                newy -= SPRITE_WIDTH/2;
            
            // add new bacteria object of the right type at (new x, new y)
            divide(newx, newy, mySW);
            // reset food eaten count
            m_foodEatenCount = 0;
        }
    
    // Step 4: check if overlapping with food
        else{
            Actor* closeFood = mySW->overlapsFood(this);
            if(closeFood){
                // overlaps with food object so increase food eaten count
                m_foodEatenCount++;
                // food object disappears
                closeFood->die();
            }
        }
    }
    
    // Steps 5 & 6: do specialized movement based on bacteria type if aggressive move hasn't happened
    if(!aggressionComplete){
        doSpecializedMovement();
    }
}

bool Bacterium::preventsLevelCompleting(){
    return true;
}

void Bacterium::damage(int harm){
    Agent::damage(harm);
    // 50% chance bacteria turns into food when dead
    if(isDead()){
        getMyStudentWorld()->increaseScore(100);
        if(randInt(0, 1)){
            StudentWorld* mySW = getMyStudentWorld();
            Food* food = new Food(getX(), getY(), mySW);
            mySW->addActor(food);
        }
    }
}

void Bacterium::doSpecializedMovement(){
    return;
}

bool Bacterium::doAggressiveThing(){
    return false;
}

int Bacterium::getMPD(){
    return m_movementPlanDistance;
}

void Bacterium::setMPD(int mpd){
    m_movementPlanDistance = mpd;
}

void Bacterium::divide(int newx, int newy, StudentWorld* sw){
    return;
}

// EColi : Bacterium
EColi::EColi(double startX, double startY, StudentWorld* sw)
:Bacterium(IID_ECOLI, startX, startY, sw, 5, 4)
{}

EColi::~EColi(){}

void EColi::playDamageSound(){
    if(isDead())
        getMyStudentWorld()->playSound(SOUND_ECOLI_DIE);
    else
        getMyStudentWorld()->playSound(SOUND_ECOLI_HURT);
}

void EColi::doSpecializedMovement(){
    StudentWorld* mySW = getMyStudentWorld();
    // get angle towards Socrates
    int dirSoc = mySW->angleToSocrates(this);
    
    int tries = 0;
    // keep trying different movement angles while blocked by dirt
    while(tries < 10 && mySW->dirtBlock(dirSoc, 2, this)){
        tries++;
        dirSoc += 10;
    }
    // if unblocked, move in that direction
    if(tries < 10)
        moveAngle(dirSoc, 2);
    
    // if blocked after 10 tries just stay still
}

void EColi::divide(int newx, int newy, StudentWorld* sw){
    EColi* newecoli = new EColi(newx, newy, sw);
    sw->addActor(newecoli);
}

// Salmonella : Bacterium
Salmonella::Salmonella(double startX, double startY, StudentWorld* sw, int hp, int socratesDamage)
:Bacterium(IID_SALMONELLA, startX, startY, sw, hp, socratesDamage){}

Salmonella::~Salmonella(){
    int foodChance = randInt(0, 1);
    if(foodChance){
        StudentWorld* mySW = getMyStudentWorld();
        Food* food = new Food(getX(), getY(), mySW);
        mySW->addActor(food);
    }
}

void Salmonella::playDamageSound(){
    if(isDead())
        getMyStudentWorld()->playSound(SOUND_SALMONELLA_DIE);
    else
        getMyStudentWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Salmonella::doSpecializedMovement(){
    StudentWorld* mySW = getMyStudentWorld();
    // continue moving in same direction as before
    if(getMPD() > 0){
        // decrement movement plan distance
        setMPD(getMPD()-1);
        
        // check if blocked by dirt or Petri dish radius
        int curDir = getDirection();
        double planx, plany;
        getPositionInThisDirection(curDir, 3, planx, plany);
        if(mySW->dirtBlock(curDir, 3, this) || mySW->euclideanDistance(planx, plany, VIEW_WIDTH/2, VIEW_HEIGHT/2) > VIEW_RADIUS){
            // blocked in some way so pick random direction
            setDirection(randInt(0, 359));
            setMPD(10);
        }
        else{
            // path not blocked so move that way
            moveAngle(curDir, 3);
        }
    }
    // find new move direction based on food availability
    else{
        // check for food within radius
        Actor* closeFood = mySW->findFood(this);
        // if food within 128 pixels
        if(closeFood){
            int dir = mySW->angleToActor(this, closeFood);
            // if unblocked, move in that direction 3 units
            if(!mySW->dirtBlock(dir, 3, this)){
                moveAngle(dir, 3);
                return;
            }
        }
        // if no food within 128 pixels, or path blocked towards food, pick random direction
        setDirection(randInt(0, 359));
        setMPD(10);
        return;
    }
}


// RegularSalmonella : Salmonella
RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* sw)
:Salmonella(startX, startY, sw, 4, 1){
    
}

RegularSalmonella::~RegularSalmonella(){}

void RegularSalmonella::divide(int newx, int newy, StudentWorld* sw){
    RegularSalmonella* newrsalm = new RegularSalmonella(newx, newy, sw);
    sw->addActor(newrsalm);
}

// AggressiveSalmonella : Salmonella
AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* sw)
:Salmonella(startX, startY, sw, 4, 2){
    
}

AggressiveSalmonella::~AggressiveSalmonella(){}

bool AggressiveSalmonella::doAggressiveThing(){
    StudentWorld* mySW = getMyStudentWorld();
    if(mySW->distanceToSocrates(this) <= 72){
        // try to move 3 pixels towards Socrates
        // calculate direction of movement
        int dirSoc = mySW->angleToSocrates(this);
        // if unblocked, move in that direction 3 units
        if(!mySW->dirtBlock(dirSoc, 3, this)){
            moveAngle(dirSoc, 3);
        }
        // if blocked by dirt, stay stuck and don't find new direction
        
        // tells bacteria's doSomething to skip further movement planning for this aggressive salmonella
        return true;
    }
    return false;
}

void AggressiveSalmonella::divide(int newx, int newy, StudentWorld* sw){
    AggressiveSalmonella* newasalm = new AggressiveSalmonella(newx, newy, sw);
    sw->addActor(newasalm);
}
