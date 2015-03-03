
#include "StudentWorld.h"
#include <string>
using namespace std;

//** Fix the fact that a robot can't move to a player's position 

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::~StudentWorld()
{
    // DOING THE SAME THING AS CLEAN UP 
    delete m_player; 
    for (vector<Actor*>::iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {          Actor* q = *p; 
            delete q;
            vector<Actor*> ::iterator g = m_actors.erase(p);
            p = g; 
            p--;
            
        }
}

int StudentWorld::init()
{
    cerr << "init function called " <<endl;
    isLevelFinished = false;
    //increaseLevel();
    string curLevel = m_level;
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
        return -1; // something bad happened!
    // otherwise the load was successful and you can access the // contents of the level – here’s an example
    if (result == Level::load_fail_file_not_found)
        cerr << "Could not find level00.dat data file\n"; else if (result == Level::load_fail_bad_format)
            cerr << "Your level was improperly formatted\n"; else if (result == Level::load_success)
                cerr << "Successfully loaded level\n";
    
    int x =0 , y = 0; Level::MazeEntry item; m_bonus = 1000;// = lev.getContentsOf(0, 0);
    for (; x < VIEW_WIDTH ; x++ )
        {
        y = 0;
        for (; y < VIEW_HEIGHT; y++)
            {  item = lev.getContentsOf(x, y);
                switch(item)
                {
                    case Level::player:
                    m_player = new Player(x,y, this) ;
                    break;
                    case Level::wall:
                    m_actors.push_back(new Wall(x,y, this));
                    break;
                    case Level::ammo:
                    m_actors.push_back(new Ammo (x,y,this)); // implement it in the goodies class
                    break;
                    case Level::boulder:
                    m_actors.push_back(new Boulder(x,y,this));
                    break;
                    case Level::hole:
                    m_actors.push_back(new Hole (x,y,this));
                    break;
                    case Level::jewel:
                    m_actors.push_back(new Jewel (x,y,this));
                    break;
                    case Level::extra_life:
                    m_actors.push_back(new ExtraLifeGoodie (x,y,this));
                    break;
                    case Level::restore_health:
                    m_actors.push_back(new RestoreHealthGoodie (x,y,this));
                    break;
                    case Level::horiz_snarlbot:
                    m_actors.push_back(new SnarlBot (x,y,Actor::right,this)) ;
                    break;
                    case Level::vert_snarlbot:
                    m_actors.push_back(new SnarlBot (x, y , Actor::down, this));
                    case Level::exit:
                    m_actors.push_back(new Exit (x,y,this));
                    break;
                    

                }
            }
        }
    
    return GWSTATUS_CONTINUE_GAME;

}

// returns true when no more jewels
bool StudentWorld::noJewels()const
{
    Actor* jewel = getJewels();
    if (jewel == nullptr)
        return true;
    else
        return false;
    
}


void StudentWorld::setLevelFinished()
{
    isLevelFinished = true;
}

void StudentWorld::increaseLevel()
{
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2)<<m_level_called << ".dat";
    m_level = oss.str();
    m_level_called++;

}
///IMPLEMENT MOVE ACCORDING TO PSEUDOCODE  ! 

int StudentWorld::move()
{
    // update game status line 
    cerr << "Move function called---" <<endl ;
    setDisplayText();
    
    // The term "actors" refers to all robots, the Player, Goodies, // Boulders, Jewels, Holes, Bullets, the Exit, etc.
   if (m_player->isAlive())     
       {     m_player->doSomething(); // ask player to do something
           for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
               {
                    Actor* p = *i;
               if (p->isAlive())
                    p->doSomething();
               } }
    
    if (!m_player->isAlive())
        {       decLives();
        return GWSTATUS_PLAYER_DIED;
        }
    
    //remove dead actors
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
        {
        Actor* p = *i;
        if (!p->isAlive())
            {
            delete p;
            vector<Actor*> ::iterator g = m_actors.erase(i); // removing the dead actors from the vector array 
            i = g; 
            i--;
            }
        }
    //reduce bonus
    if (m_bonus > 0)
    m_bonus--;
    
    // if player has collected all the jewels- make the exit active
    if (noJewels())
        {
            Actor* exit = returnExit();
            if (exit != nullptr)
                {   exit->setVisible(true);
                    playSound(SOUND_REVEAL_EXIT);
                }
        }
    
    // return the proper result
    if (!m_player->isAlive())
        return GWSTATUS_PLAYER_DIED;
    if (isLevelFinished)
        {
           // increase score done by exit 
        cerr << "finished level--- " <<endl;
        return GWSTATUS_FINISHED_LEVEL;
        }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    cerr << "Cleaning up -- " <<endl;
    {
    // DOING THE SAME THING AS CLEAN UP 
   delete m_player; 
    for (vector<Actor*>::iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {          Actor* q = *p; 
            delete q;
            vector<Actor*> ::iterator g = m_actors.erase(p);
            p = g; 
            p--;}
    
    }
    
}

int StudentWorld::whatObject(int x, int y) 
{
    int ID;
    Actor* p_actor; 
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
        {
        p_actor = *i;
        if (p_actor->getX() == x && p_actor->getY() == y) // if the coordinates match then get id 
            {     ID = p_actor->getID();
                switch (ID) {
                    case IID_WALL:
                        return IID_WALL;
                        break;
                    case IID_PLAYER:
                        return IID_PLAYER;
                        break;
                    case IID_AMMO:
                        return IID_AMMO;
                        break;
                    default:
                        
                        break;
                }
            }
        }
    return -1;
}

// returns null if actor not found otherwise returns the actor at a given location 
Actor* StudentWorld::getActorAtLocation(int x , int y)const
{
    for(vector<Actor*>::const_iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {
            Actor* curr = *p;
        if (curr->getX() == x && curr -> getY()== y)
            {
                return curr;
            }
        }
    return nullptr;
}

// add an actor to student world 
void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a); 
}




void StudentWorld::Player_Add_Ammunition (unsigned int howMuch) 
{

    m_player->addAmmo(howMuch);
}

Coord StudentWorld::getPlayerPosition()const
{
    Coord position; position.m1_x = m_player->getX(); position.m1_y = m_player->getY();
    return position;
}

bool StudentWorld::canAgentMoveTo(Agent *agent, int x, int y, int dx, int dy) const // takes in the current value of position and direction 
{

    Actor* ap = getActorAtLocation(x+dx,y+dy);
    if (agent->canPushBoulders() && ap != nullptr)
        {
         cerr <<"Found a boulder" <<endl;
            Boulder* bp = dynamic_cast<Boulder*>(ap);
            if (bp!= nullptr) // if null goes inside the next if statement
                {  cerr << "pushing a boulder" <<endl;
                   if (bp->push(dx+dx,dy+dy)) // the direction the player + boulder
                    return true;
                    else return false;}
        }
     if (ap != nullptr && !(ap->allowsAgentColocation()))
        { cerr << "Didn't find boulder" << endl;
            return false;}
        //if (m_player->getX() == (x+dx) || m_player->getY() == (y+dy)) // robot can't move to player's postion
      //  return false;
  
    return true;
}

void StudentWorld::decreaseAmmo()
{
    m_player->decreaseAmmo();
}



Actor* StudentWorld::getNonBulletActorAtLocation(int x, int y)
{   
    for(vector<Actor*>::const_iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {
        Actor* curr = *p;
        if (curr->getX() == x && curr -> getY()== y && curr->getID() != IID_BULLET)
            {
            return curr;
            }
        }
    return nullptr;

}

Actor* StudentWorld::getNonRobotActorAtLocation(int x, int y)
{   
    for(vector<Actor*>::const_iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {
        Actor* curr = *p;
        if (curr->getX() == x && curr -> getY()== y && curr->getID() != IID_SNARLBOT)
            {
            return curr;
            }
        }
    return nullptr;
    
}

Actor* StudentWorld::getJewels() const

{
    for(vector<Actor*>::const_iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {
        Actor* curr = *p;
        if (curr->getID() == IID_JEWEL)
            {
            return curr;
            }
        }
    return nullptr; // no jewels 
    
    
}

Actor* StudentWorld::returnExit()
{
    for(vector<Actor*>::const_iterator p = m_actors.begin(); p !=m_actors.end(); p++)
        {
        Actor* curr = *p;
        if (curr->getID() == IID_EXIT)
            {
            return curr;
            }
        }
    return nullptr; // no exit 

}

/// Formatting stuff for the screen 
std::string StudentWorld:: formatStuff (int score, int level, unsigned int bonus, int livesLeft, unsigned int hitPoints, unsigned int ammo)
{
    ostringstream oss; 
    oss.fill('0');
    oss << "Score: " << setw(7) << score << "  "  << "Level: " << setw(2) <<level << "  " ;
    oss.fill();
    oss << "Lives: " << setw(2) << livesLeft << "  " << setw(3) << "Health: " << hitPoints << "%  " 
    << "Ammo: " << setw(3) << ammo << "  " << "Bonus: " << setw(4) << bonus;
    
    return oss.str();
}

void StudentWorld::setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = m_bonus;
    int livesLeft = getLives();
    unsigned int hitPoints = m_player->getHealthPct();
    int ammo = m_player->getAmmo();
    std::string s = formatStuff(score, level, bonus, livesLeft, hitPoints, ammo);
    setGameStatText(s);
}


