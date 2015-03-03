#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
//#include "StudentWorld.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// IMPLEMENT SOUND 
//const int DEFAULT_NONE_IMAGEID = 14;
struct Coord // to help organize the Actor positions
{
    int m1_x;
    int m1_y;
};
class Agent;
class StudentWorld;
class Actor : public GraphObject
{
    public :
	Actor(int x, int y, int IMAGEID, StudentWorld* sW, Direction direction = none , int hitPoints = 0);
    virtual ~Actor();
	virtual void doSomething() = 0 ; // pure virtual since different actors implement this differently
    bool isAlive();
    void setDead(); 
    void setDirection(Direction d); 
   // void setPostion (int x, int y);
	//Coord getPosition();
    // Reduce this actor's hit points
    void decHitPoints(unsigned int amt);
    
    // Get this actor's world
    StudentWorld* getWorld() const;
    
    // Can an agent occupy the same square as this actor?
    virtual bool allowsAgentColocation() const;
    
    // Can a boulder occupy the same square as this actor?
    virtual bool allowsBoulder() const;
    
    // Does this actor count when a factory counts items near it?
    virtual bool countsInFactoryCensus() const
    { return false;}
    
    // Does this actor stop bullets from continuing?
    virtual bool stopsBullet() const;
    
    // How many hit points does this actor have left?
    virtual unsigned int hitPointsLeft() const
    { return m_hitPoints;}
    
    // Set this actor's hit points to amt.
    virtual void getHitPoints(int amt)
    {   m_hitPoints = amt; }

    // Can this actor be damaged by bullets?
    virtual bool isDamageable() const
    { return false; } 
    
    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(unsigned int damageAmt)
    {  if (isDamageable())
        m_hitPoints-= damageAmt; }
    
    // Can this actor be pushed by a to location x,y?
    virtual bool bePushedBy(Agent* a, int x, int y)
    { return false; }
    
    // Can this actor be swallowed by a hole?
    virtual bool isSwallowable() const
    { return false;}
    
    // Can this actor be picked up by a KleptoBot?
    virtual bool isStealable() const
    { return false; }
        
    int returnImageID () const
    { return getID();}
    
   
    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(unsigned int damageAmt); 
    
    Coord convertDirection(Actor::Direction d);
    

private:
    bool is_Alive;
    StudentWorld* m_studentWorld; 
    unsigned int m_hitPoints;
   };

class Agent : public Actor
{
public:
    Agent(int startX, int startY, int imageID,StudentWorld* world, 
          Direction startDir, unsigned int hitPoints);
    
    // Move to the adjacent square in the direction the agent is facing
    // if it is not blocked, and return true.  Return false if the agent
    // can't move.
    bool moveIfPossible(Direction d);
    // Is a robot or player damageable
    
    virtual bool isDamageable() const
    {return true;}
    
    virtual bool allowsAgentColocation()const
    { return false;}
    // Return true if this agent can push boulders (which means it's the
    // player).
    virtual bool canPushBoulders() const
    { return true; }
    /*
    // Return true if this agent can push boulders (which means it's the
    // player).
    virtual bool needsClearShot() const;
    
    // Return the sound effect ID for a shot from this agent.
    // player).
    virtual int shootingSound() const;*/
//private:
   //int m_hitPoints;
};


class Wall : public Actor
{
    public :
	Wall (int x, int y , StudentWorld* sW);
    virtual void doSomething();
    virtual bool isDamageable()const
    {
        return false;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Player
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class Player : public Agent
{
public:
    Player(int x, int y, StudentWorld* studentWorld);
    void doSomething();
    //StudentWorld* getWorld();
    void addAmmo(unsigned int ammo);
    virtual bool canPushBoulders()
    { return true;}
    
   // Get player's health percentage
    unsigned int getHealthPct() const
    { return ((hitPointsLeft()*100)/20);}
    
    // Get player's amount of ammunition
    unsigned int getAmmo() const
    {return roundsOfAmmo;}
    
    // Restore player's health to the full amount.
    void restoreHealth();
    
    // Increase player's amount of ammunition.
    void increaseAmmo();
    
    virtual bool isDamageable()const 
    { return true;}
    virtual bool stopsBullet()
    { return true;}
    // Decrease player's ammunition by 1 
    void decreaseAmmo()
    {   if (roundsOfAmmo > 0)
            roundsOfAmmo--;}
    virtual bool tryToBeKilled(unsigned int damageAmt); 
    virtual bool allowsAgentColocation () const
    { return false; } 

private:
    int roundsOfAmmo;
};

// Boulders 

class Boulder : public Actor
{
public: 
    Boulder (int x, int y, StudentWorld* studentWorld);
    void doSomething();  
    virtual bool isDamageable() const
    { return true; } 
   // virtual void damage(unsigned int damageAmt);
    virtual bool isSwallowable() const
    { return true;}                    // can be swallowed by a hole
    virtual bool bePushedBy(Player* a, int x, int y); // can it be pushed by a robot?
    bool push(int dx, int dy);
private:
    //StudentWorld* m_studentWorld; // need this to access player's position
    int hitPoints;
};

// Holes
class Hole : public Actor
{
public:
    Hole( int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool allowsBoulder() const
    { return true;}
    virtual bool stopsBullet() const
    { return false;}
};

// Bullets 

class Bullet : public Actor
{
public:
    Bullet(int startX, int startY, StudentWorld* world, Direction startDir);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const
    { return true; } 
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Robots
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/* pure virtual*/
class Robot : public Agent
{
public:
    Robot (int startX, int startY, int imageID, StudentWorld* world,
          unsigned int hitPoints, unsigned int score, Direction startDir);
    virtual void doSomething() const; 
    virtual bool isDamageable() const
    { return true; } 
    virtual bool tryToBeKilled(unsigned int damageAmt)
    { return false;} // some implementation we are never going to follow
    virtual bool canPushBoulders() const
    { return false; }
    virtual bool needsClearShot() const
    { return true;}
   //  virtual int shootingSound() const; 
    // Does this robot shoot?
    virtual bool isShootingRobot() const
    { return false; } // base case robot cannot shoot 
    int getTicks()
    { return m_ticks;} 
    void resetTicks(int levelNum)
    {     m_ticks  = ((28- levelNum)/4);
    }
    void decrementTicks()
    {
    m_ticks--;
    }
    
private:
    int m_ticks;
};
// SnarlBots
class SnarlBot : public Robot 
{
public:
    SnarlBot(int startX, int startY, Direction startDir, StudentWorld* world);
    virtual void doSomething();
   // virtual void damage(unsigned int damageAmt);
    virtual bool tryToBeKilled(unsigned int damageAmt); 
private:
    int m_ticks;
};

// KleptoBots

class KleptoBot : public Robot
{
public:
    KleptoBot(int startX, int startY, int imageID, StudentWorld* world,
              unsigned int hitPoints, unsigned int score);
   // virtual ~KleptoBot ();
    virtual void doSomething();
    virtual bool countsInFactoryCensus() const;
    virtual bool tryToBeKilled(unsigned int damageAmt);
private:
    int distanceBeforeTurning;
    int goodiePicked; 
    int goodieScore;
};
class RegularKleptoBot : public KleptoBot
{
    
};

class AngryKleptoBot : public KleptoBot
{
    
};

///////////////////////////////
/////////// Kleptobot Factories   
///////////////////////////////

class KleptoBotFactory : public Actor
{
public:
    enum ProductType { REGULAR, ANGRY };
    
    KleptoBotFactory(int startX, int startY, ProductType type, StudentWorld* world);
    virtual void doSomething();
    virtual bool stopsBullet() const
    { return true;}
};

////////////////////////////
/////////// Pickupable Items  
////////////////////////////

class PickupableItem : public Actor
{
public:
    PickupableItem(int startX, int startY, int imageID, StudentWorld* world,
                   unsigned int score);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const
    { return true; } 
    virtual bool stopsBullet()const
    { return false;}
    int getScore()
    { return score;}
private:
    int score; 
    
};

class Jewel : public PickupableItem
{
public:
    Jewel(int startX, int startY, StudentWorld* world);
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Goodies
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Goodie : public PickupableItem
{
public:
    Goodie(int startX, int startY, int imageID, StudentWorld* world,
           unsigned int score);

    virtual void doSomething();
    virtual bool isStealable() const
    { return true;}
    // Set whether this goodie is currentluy stolen.
    void setStolen(bool status);
private:
    bool isStolen;
};


class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world);
    virtual void doSomething(); 
};


class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(int startX, int startY , StudentWorld* world);
    virtual void doSomething(); 
};

class Ammo : public Actor
{
public:
    Ammo (int x, int y, StudentWorld* sW);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const
    {
        return true;
    }
private:
    //bool is_Alive;
    //StudentWorld* m_Ammo_studentWorld;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Exit 
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Exit : public Actor
{
public:
    Exit(int startX, int startY,StudentWorld* world);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const
    { return true;} 
    virtual bool stopsBullet()const
    { return false; } 
};


#endif // ACTOR_H_
