#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
// Fix bullet not recieving itself back 


/* Graph object lets you
 GraphObject(int imageID, int startX, int startY,
 Direction startDirection = none);
 void setVisible(bool shouldIDisplay); void getX() const;
 void getY() const;
 void moveTo(int x, int y);
 Direction getDirection() const; // Directions: none, up, down, left, right void setDirection(Direction d); // Directions: none, up, down, left, right
 */
/////////////////////////////
/////////// Base Class- Actor
////////////////////////////
Actor::Actor(int x, int y, int IMAGEID, StudentWorld* sW, Direction direction, int hitPoints) : GraphObject(IMAGEID,x,y,direction),is_Alive(true), m_studentWorld(sW), m_hitPoints(hitPoints)
{
    setVisible(true);
}

Actor::~Actor()
{
    setVisible(false);
}



bool Actor::isAlive()
{
    return is_Alive;
}

void Actor::setDead()
{
    is_Alive = false;
}

void Actor::setDirection(Direction d)
{
    GraphObject::setDirection(d);
}

void Actor::decHitPoints(unsigned int amt)
{
    m_hitPoints = m_hitPoints-amt;
}

StudentWorld* Actor::getWorld() const
{
    return m_studentWorld;
}

bool Actor::allowsAgentColocation()const // for the default actor you can't have an agent existing in the same place
{
    return false; 
}

bool Actor::allowsBoulder()const
{
    return false;
}

bool Actor::stopsBullet()const
{
    return true;
}




// If dies then dies otherwise gets damage - Returns false if damaged but not dead 
bool Actor:: tryToBeKilled(unsigned int damageAmt) 
{
   if (isDamageable())
       {  if (m_hitPoints - damageAmt <= 0 || m_hitPoints - damageAmt > 20) // if damage greater than hitPoints remaining then die
           {  m_hitPoints -= damageAmt; setDead();
            return true;}
       else
           damage(damageAmt);
       }
    return false;
}
/////////////////////////////
/////////// Agent 
////////////////////////////

Agent::Agent(int startX, int startY, int imageID,StudentWorld* world, 
             Direction startDir, unsigned int hitPoints) : Actor(startX, startY, imageID, world, 
                                                                startDir, hitPoints)
{}
// *** Be careful ! may need to switch these around if moves in the opposite direction
bool Agent::moveIfPossible(Direction d)
{
    StudentWorld* sW = getWorld(); // get world from the base class 
    int dx,dy;
    switch (d) {
        case GraphObject::left:
            dx = -1;
            dy = 0;
            break;
          case GraphObject::right:  
            dx = 1; 
            dy = 0; 
            break;
        case GraphObject::down:
            dx = 0; dy = -1; // down and up is switched because for some reason the executable behaves weird
            break; 
        case GraphObject::up:
            dx = 0; dy = 1; 
            break;
        default:
            break;
    }
   if(sW->canAgentMoveTo(this,getX() , getY(), dx, dy))
       {  moveTo(getX()+ dx, getY()+ dy);
           return true;}
    
    
    return false;
}

/////////////////
/// Robots 
////////////////

Robot::Robot(int startX, int startY, int imageID, StudentWorld* world,
             unsigned int hitPoints, unsigned int score, Direction startDir)
:Agent(startX, startY, imageID, world, startDir, hitPoints)
{
}

void Robot::doSomething()const
{
    ;
}
 
/////////////////
/// Snarlbot 
////////////////

SnarlBot::SnarlBot(int startX, int startY, Direction startDir, StudentWorld* world):
Robot(startX, startY, IID_SNARLBOT, world, 10, 100, startDir)
{
    int level = getWorld()->getLevel();
    resetTicks(level);}

//need to implement it for vertical robots *******NEEDS FIXING 
// BUG: ALLOWING AGENT COLOCATION AND ALSO NOT RELEASING BULLET IF PLAYER OCCUPIES THE NEXT SPOT (CHECK THAT)
void SnarlBot::doSomething()
{

    if (!isAlive())
        return;
    if (getTicks() > 1)
        {  decrementTicks(); return;}
    if (getTicks() == 1)
        {
        StudentWorld* myWorld = getWorld(); bool didShoot = false;
        Direction dir = getDirection();
        Coord d  = convertDirection(dir);
        Coord playerd = convertDirection(myWorld ->getPlayer()->getDirection());
                    // when only one tick left before counter resets - move/determine whether to fire   
            // FIX THIS LATER
            if (dir == left || dir == right)
                {
                    // if row matches + same direction and no obstacle in the way then fire bullet 
                    if (myWorld->getPlayerPosition().m1_y == getY() 
                        && ( playerd.m1_x == -(d.m1_x)|| (d.m1_x) == -playerd.m1_x) ) // player's row and direction opposite
                        { int i; 
                        // if no obstacle in the way - check it according to the direction provided 
                            for (i = getX(); i != myWorld->getPlayerPosition().m1_x ; i = i+d.m1_x)
                                {
                                 Actor* a = myWorld-> getNonRobotActorAtLocation(i, getY ()); // get actors to the left 
                                    if (a!= nullptr)    
                                        { if (a->stopsBullet())
                                            break;}
                                    }
                                if (i == myWorld->getPlayerPosition().m1_x) // no obstacle found 
                            {  
                                myWorld->addActor(new Bullet((getX()+d.m1_x), (getY()+d.m1_y), getWorld(), dir));
                                didShoot = true;
                            }
                            
                        }
                    // if doesn't shoot then moves
              if (!didShoot)
            {  if (myWorld->getActorAtLocation(getX()+d.m1_x, getY())== nullptr
                   || myWorld->getActorAtLocation(getX()+d.m1_x, getY())->allowsAgentColocation())
                moveTo(getX()+d.m1_x, getY());
                else
                    {
                        if (dir == left)
                            setDirection(right);
                        else if (dir == right)
                            setDirection(left);
                    }
            }
                
                }
        // vertical snarlbots           
        if (dir == up || dir == down)
            {
            // if col matches + same direction and no obstacle in the way then fire bullet 
              if (myWorld->getPlayerPosition().m1_x == getX() 
             && ( playerd.m1_y == -(d.m1_y)|| (d.m1_y) == -playerd.m1_y) ) // player's row and direction opposite
             { int i; 
             // if no obstacle in the way - check it according to the direction provided 
             for (i = getY(); i != myWorld->getPlayerPosition().m1_y ; i = i+d.m1_y)
             {
             Actor* a = myWorld-> getNonRobotActorAtLocation(getX(),i); // get actors to the left 
             if (a!= nullptr)    
             { if (a->stopsBullet())
             break;}
             }
             if (i == myWorld->getPlayerPosition().m1_y) // no obstacle found 
             {  
             myWorld->addActor(new Bullet((getX()+d.m1_x), (getY()+d.m1_y), getWorld(), dir));
             didShoot = true;
             }
             
             }
            // if doesn't shoot then moves
            if (!didShoot)
            {  if (myWorld->getActorAtLocation(getX(), getY()+d.m1_y)== nullptr
                 || myWorld->getActorAtLocation(getX(), getY()+d.m1_y)->allowsAgentColocation())
                moveTo(getX()+d.m1_x, getY()+d.m1_y);
                else
                {
                if (dir == up)
                  setDirection(down);
                else if (dir == down)
                  setDirection(up);
                }
            }
            
            
            }

        resetTicks(getWorld()->getLevel()); // if last tick then reset the ticks
        }
}


bool SnarlBot::tryToBeKilled(unsigned int damageAmt)
{
    if (isDamageable())
        {  if (hitPointsLeft() - damageAmt <= 0) // if damage greater than hitPoints remaining then die
            {   setDead(); getWorld()->increaseScore(100); getWorld()->playSound(SOUND_ROBOT_DIE);
                return true;}
        else
            { damage(damageAmt); getWorld()->playSound(SOUND_ROBOT_IMPACT);}
        }
    return false;
}

//////////////////////////////
//////////////// Kleptobot 
//////////////////////////////
KleptoBot::KleptoBot(int startX, int startY, int imageID, StudentWorld* world,
                unsigned int hitPoints, unsigned int score): Robot(startX, startY,IID_KLEPTOBOT, world, 5 , 10, right)
{
    distanceBeforeTurning = rand() % 6; 
}



void KleptoBot::doSomething() 
{
    if (!isAlive())
        return;
    if (getTicks()>1)
        {  decrementTicks(); return;}
    if (getTicks() == 1)
        {   StudentWorld* myWorld = getWorld();
            Actor* goodie = myWorld->getNonRobotActorAtLocation(getX(), getY()); // if actor at my location a goodie 
            Goodie* goodie_cast = dynamic_cast<Goodie*>(goodie);
            if (goodie_cast != nullptr)// goodie on my location
                {
                    goodiePicked = goodie_cast->returnImageID();
                    goodieScore = goodie_cast-> getScore(); // storing its imageID and score
                    goodie_cast->setDead();
                    myWorld->playSound(SOUND_ROBOT_MUNCH);
                }
            else if (distanceBeforeTurning > 0)
                        {
                            Coord direction = convertDirection(getDirection());
                        Actor* a = myWorld->getNonRobotActorAtLocation(getX()+direction.m1_x, getY()+direction.m1_y);
                        if (a == nullptr || a->allowsAgentColocation())
                            {  moveIfPossible(getDirection()); distanceBeforeTurning--;}
                            
                        }
            else 
                {
                    distanceBeforeTurning = rand()%6;
                int i = 4; bool moved = false;
                    while(i < 4)
                        {
                            moved = moveIfPossible(left);
                            if (moved)
                                {   setDirection(left);
                                    break;}
                        moved = moveIfPossible(right);
                        if (moved)
                            {   setDirection(right);
                                break;}                  
                        moved = moveIfPossible(up);
                        if (moved)
                            {   setDirection(up);
                                break;}
                        moved = moveIfPossible(down);
                        if (moved)
                            {   setDirection(down);
                                break;}
                        }
                    if (!moved)
                        {
                        setDirection(left);
                        }
                }
            }
                        
                
            
        resetTicks(getWorld()->getLevel());
    }

bool KleptoBot::tryToBeKilled(unsigned int damageAmt)
{
    if (isDamageable())
        {  if (hitPointsLeft() - damageAmt <= 0) // if damage greater than hitPoints remaining then die
            {  getWorld()->addActor(new Goodie (getX(),getY(),goodiePicked,getWorld(),goodieScore)); setDead();
                getWorld()->increaseScore(100); getWorld()->playSound(SOUND_ROBOT_DIE);
                return true;}
        else
            {damage(damageAmt); getWorld()->playSound(SOUND_ROBOT_IMPACT);}
        }
    return false;
}

bool KleptoBot::countsInFactoryCensus()const
{
    // if factory nearby return true
    return true;
}


//////////////////////////////
/////////// Kleptobot Factory
//////////////////////////////
KleptoBotFactory::KleptoBotFactory(int startX, int startY, ProductType type, StudentWorld* world) : Actor(startX,startY,IID_ROBOT_FACTORY, world)
{
}

void KleptoBotFactory::doSomething()
{
    
}

///////////////////////////
/////////// Wall 
////////////////////////////
Wall::Wall(int x, int y, StudentWorld* sW) : Actor(x, y,IID_WALL, sW,none, 0)
{
   // ImageID = IID_WALL;
    setVisible(true);
} // #dont think you need imageID#

void Wall:: doSomething()
{
    ; // wall does nothing
    // #try to make it more efficient#
}

////////////////////////////
/////////// Player 
////////////////////////////

Player::Player(int x, int y, StudentWorld* studentWorld) : Agent(x,y,IID_PLAYER, studentWorld, right, 20), roundsOfAmmo(20)  // find a way of loading x and y coordinates from the student world init function
{}

void Player::doSomething()
{
    Bullet* b = dynamic_cast<Bullet*>(getWorld()->getActorAtLocation(getX(), getY())); // if bullet occupies the same spot as player 
    if (b != nullptr)
        { bool getsKilled = tryToBeKilled(2); b->setDead();
        if (getsKilled)
            this->setDead();}
        
if (isAlive())
{    int ch;
    if (getWorld()->getKey(ch)) // THIS IS A PROBLEM ! 
        {
            // user hit a key this tick !
       // Coord position = getPosition();
       // Actor* ap = getWorld()-> getActorAtLocation(position.m1_x, position.m1_y);
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_LEFT: 
                moveIfPossible(left);setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                moveIfPossible(right);setDirection(right);
                break;
            case KEY_PRESS_UP:
                moveIfPossible(up);setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                moveIfPossible(down);setDirection(down);
                break;
            case KEY_PRESS_SPACE: // fire bullets and shit
               if (roundsOfAmmo > 0)
            {  getWorld()->addActor(new Bullet(getX(), getY(), getWorld(), getDirection()));// starting at the same position as player
                decreaseAmmo();
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                cerr << "Played Sound" << endl;}
                break;
            default:
                break;
        }
        
        }
}
}
void Player::addAmmo(unsigned int ammo)
{
    roundsOfAmmo+= ammo;
}

bool Player:: tryToBeKilled(unsigned int damageAmt)
{
   bool b = Actor::tryToBeKilled(damageAmt);
    return b;
}

////////////////////////////
/////////// Boulders
////////////////////////////

Boulder::Boulder(int x, int y, StudentWorld* studentWorld): Actor(x,y,IID_BOULDER,studentWorld, none, 10)
{
}

void Boulder::doSomething()
{
    ;
}

// returns true when either empty space or allows boulder then return true
bool Boulder::bePushedBy(Player *a, int x, int y) // assume x and y provided are the target location - rememeber how in player's case it was player's location
{
    if (a->canPushBoulders())
        {
        Actor* ap = getWorld()->getActorAtLocation(x, y);
        if (ap != nullptr && !(ap->allowsBoulder()))  // not empty and doesn't allow boulder
            {
            cerr << "Not empty and doesn't allow boulder" << endl;
            return false;
            }
        if (ap!= nullptr && ap->allowsBoulder())
            {cerr << "Hole Found" << endl;
                ap->setDead();
                this->setDead();
                return true;}
        if (ap == nullptr)
            return true;
        }
    cerr << "returned for something else" <<endl;
    return false;
}

bool Boulder::push(int dx, int dy)
{
    // change this
    StudentWorld* sW = getWorld(); // get world from the base class 
    Player* p = sW->getPlayer();
   Coord position= sW->getPlayerPosition();
    if(bePushedBy(p, position.m1_x+ dx, position.m1_y+ dy)) 
        { moveTo(position.m1_x+dx, position.m1_y+dy);
            return true;}
    return false;
    
}
////////////////////////////
/////////// Hole 
////////////////////////////

Hole::Hole(int startX, int startY, StudentWorld* world):Actor(startX, startY, IID_HOLE,world)
{}

void Hole::doSomething()
{
    if (!isAlive())
        return; // boulder-hole meeting implementation in Boulder's be pushed by function 
}

///////////////
//  Bullets
///////////////

Bullet::Bullet(int startX, int startY, StudentWorld* world, Direction startDir) : 
Actor(startX, startY,IID_BULLET, world,  startDir)
{
}

void Bullet::doSomething()
{
    if (!isAlive())
        return;
    int dx = 0; int dy = 0;
    switch (getDirection()) {
        case GraphObject::left:
            dx = -1; 
            break;
        case GraphObject::right:
            dx = 1;
            break;
        case GraphObject::up:
            dy = 1;
            break;
        case GraphObject::down:
            dy = -1; 
            break;
        default:
        break;}
    StudentWorld* myWorld = getWorld();
 // passing the bullet's location to an actor vector to tell if colocated
    int n = 0;
   while (n<2) 
    {   Actor* ap = myWorld->getNonBulletActorAtLocation(getX(), getY()); // get actor in location its about to move
        // HOW would this work with robots?
        if (ap!=nullptr)
            { // figure out how to damage the robot when colocated with factory
                if (ap->isDamageable() && ap->stopsBullet()) 
                    { ap->tryToBeKilled(2); setDead();}  // try to be killed ensures if object killed or damaged
                if (! ap->isDamageable() && ap->stopsBullet())
                    { setDead();}
                if (! ap->isDamageable() && !ap->stopsBullet())
                    { ; } // do nothing if not damageable and doesn't stop bullet
                    
            }
        if (n==1)
            break;
    // Move itself
        int m_x = getX(); int m_y = getY();
        moveTo(m_x+dx, m_y+dy);
        n++;
            }    
}


////////////////////////////
// Pickable Items
////////////////////////////
PickupableItem::PickupableItem(int startX, int startY, int imageID, StudentWorld* world,
                               unsigned int score):Actor(startX, startY,imageID,world), score(score)
{}

void PickupableItem::doSomething()
{
    StudentWorld* myWorld = getWorld();
    if (isAlive() && getX() == myWorld->getPlayerPosition().m1_x && getY() == myWorld->getPlayerPosition().m1_y)
        {
            myWorld->increaseScore(score);
            setDead();
        }
}
////////////////////////////
// Jewel 
////////////////////////////
Jewel::Jewel(int startX, int startY, StudentWorld* world): PickupableItem(startX, startY, IID_JEWEL, world, 50)
{
}


/////////////////////
/////////// Goodies 
////////////////////////////

Goodie::Goodie(int startX, int startY, int imageID, StudentWorld* world,
               unsigned int score) : PickupableItem(startX, startY,imageID,world,score)
{}

void Goodie::doSomething()
{
    PickupableItem::doSomething();
}

void Goodie::setStolen(bool status)
{
    isStolen = status;
}
ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* world): 
Goodie(startX,startY,IID_EXTRA_LIFE,world, 1000)
{}

void ExtraLifeGoodie:: doSomething()
{
    
    StudentWorld* myWorld = getWorld();
    if (isAlive() && getX() == myWorld->getPlayerPosition().m1_x && getY() == myWorld->getPlayerPosition().m1_y)
        {
            PickupableItem::doSomething();
            getWorld()->incLives();
        getWorld()->playSound(SOUND_GOT_GOODIE);

        }
    
}


RestoreHealthGoodie::RestoreHealthGoodie(int startX, int startY, StudentWorld* world): 
Goodie(startX,startY,IID_RESTORE_HEALTH,world, 500)
{}

void RestoreHealthGoodie:: doSomething()
{
    
    StudentWorld* myWorld = getWorld();
    if (isAlive() && getX() == myWorld->getPlayerPosition().m1_x && getY() == myWorld->getPlayerPosition().m1_y)
        {
        PickupableItem::doSomething();
        getWorld()->getPlayer()->getHitPoints(20);
        getWorld()->playSound(SOUND_GOT_GOODIE);}
    
}


Ammo::Ammo(int x, int y , StudentWorld* sW):Actor(x,y, IID_AMMO,sW)
{
    setVisible(true);
}

void Ammo:: doSomething()
{   
    
    StudentWorld* myWorld = getWorld();
    if (isAlive() && getX() == myWorld->getPlayerPosition().m1_x 
        && getY()== myWorld->getPlayerPosition().m1_y) // If ammo is alive and the player steps on it
        {   myWorld->increaseScore(100);
            setDead(); // if the player picks up the ammo 
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->Player_Add_Ammunition(20);
        }
        
}

//////////////////////////////
//////////////// Exit 
//////////////////////////////

Exit::Exit(int startX, int startY,StudentWorld* world): Actor(startX,startY,IID_EXIT,world)
{
    setVisible(false);// invisible by default 
}

void Exit::doSomething()
{
    StudentWorld* myWorld = getWorld();
    Player* myPlayer = myWorld->getPlayer();
    if (this->isVisible() && myPlayer->getX() == getX() && myPlayer->getY()== getY())
        {
            myWorld->playSound(SOUND_FINISHED_LEVEL);
            myWorld->increaseScore(2000);
            myWorld->setLevelFinished(); // need to figure how to do this 
            myWorld->increaseLevel();
            myWorld->addBonusToScore();
            
        }
}



///helper function
Coord Actor :: convertDirection(Actor::Direction d)
{
    Coord result; 
    switch (d) {
        case GraphObject::left:
            result.m1_x = -1;
            result.m1_y = 0 ; 
            break;
        case GraphObject::right:
            result.m1_x = 1;
            result.m1_y = 0 ; 
            break;
        case GraphObject::up:
            result.m1_x = 0;
            result.m1_y = 1 ; 
            break;
        case GraphObject::down:
            result.m1_x = 0;
            result.m1_y = -1 ; 
            break;
        default:
            break;
    }
    return result;
}
