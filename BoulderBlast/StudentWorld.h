#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_


#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    
// Compulsory Functions
	StudentWorld(std::string assetDir)
    : GameWorld(assetDir), m_bonus(0), m_level_called(1),m_level("level00.dat"),isLevelFinished(false) //m_player(nullptr)
    {
	}
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
// Additional Helper Functions    
    
    int whatObject(int x, int y);
    void Player_Add_Ammunition (unsigned int howMuch);
    Coord getPlayerPosition()const;
    // Get a pointer to the player 
    Player* getPlayer()
    { return m_player;} 
    //Actor* getPlayerLocation(); // 
    // what actor is there at a given position 
    Actor* getActorAtLocation(int x, int y)const;
    // Can agent move to x,y?  (dx and dy indicate the direction of motion)
    bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const;
    // Can a boulder move to x,y?
    bool canBoulderMoveTo(int x, int y) const;
    
    // Is the player on the same square as an Actor?
    bool isPlayerColocatedWith(int x, int y) const;
    
    // Try to cause damage to something at a's location.  (a is only ever
    // going to be a bullet.)  Return true if something stops a --
    // something at this location prevents a bullet from continuing.
    bool damageSomething(Actor* a, unsigned int damageAmt);
    
    // Swallow any swallowable object at a's location.  (a is only ever
    // going to be a hole.)
    bool swallowSwallowable(Actor* a);
    
    // If a bullet were at x,y moving in direction dx,dy, could it hit the
    // player without encountering any obstructions?
    bool existsClearShotToPlayer(int x, int y, int dx, int dy) const;
    
    // If a factory is at x,y, how many items of the type that should be
    // counted are in the rectangle bounded by x-distance,y-distance and
    // x+distance,y+distance?  Set count to that number and return true,
    // unless an item is on the factory itself, in which case return false
    // and don't care about count.  (The items counted are only ever going
    // KleptoBots.)
    bool doFactoryCensus(int x, int y, int distance, int& count) const;
    
    // If an item a that can be stolen is at x,y, return a pointer to it;
    // otherwise, return a null pointer.  (Stealable items are only ever
    // going be goodies.)
    Actor* getColocatedStealable(int x, int y) const;
    
    // Restore player's health to the full amount.
    void restorePlayerHealth();
    
    // Increase the amount of ammunition the player has
    void increaseAmmo();
    
    // Are there any jewels left on this level?
    bool noJewels() const;
    
    // Reduce the count of jewels on this level by 1.
    bool decJewels() const;
    
    // Indicate that the player has finished the level.
    void setLevelFinished();
    
    // Decrease the amount of ammunition the player has 
    void decreaseAmmo();
    
    // Add an actor to the world
    void addActor(Actor* a);
    // get a particular kind of actor at a location
    Actor* getNonBulletActorAtLocation(int x, int y);
    Actor* getNonRobotActorAtLocation(int x, int y);
    Actor* getJewels ()const ;
    void addBonusToScore()
    {
        increaseScore(m_bonus);
        m_bonus = 0; 
    }
    Actor* returnExit();
    void increaseLevel ();
    
    
// Text Display 
    std::string formatStuff (int score, int level, unsigned int bonus, int livesLeft, unsigned int hitPoints, unsigned int ammo);
    void setDisplayText();
private:
    vector<Actor*> m_actors;
    Player* m_player;
    int m_bonus; 
    std::string m_level;
    int m_level_called;
    bool isLevelFinished;
};

#endif // STUDENTWORLD_H_
