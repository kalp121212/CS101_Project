#include <simplecpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
using namespace simplecpp;

//movingObject.h
#ifndef _MOVINGOBJECT_INCLUDED_
#define _MOVINGOBJECT_INCLUDED_

#include <simplecpp>
#include <vector>
#include <composite.h>
#include <sprite.h>

//Initialising the required global variables

int score=0;   // To store score of the player
int level=1;   //To store level the player is in
int timer=60;   //to display the time remaining for a particular level
bool yanked = false; // to check whether the lasso got yanked or no
int got_hit=0;   //to find out the number of times bomb was hit

using namespace simplecpp;

//To create linked list which stores points earned in each level, which helps us to subtract points easily when bomb is hit
struct scores{
    int score;
    scores *next;
};

//Class with details of the player
class player{

    char name[100];
    int points;

    public:

    void input(int level){
            system("CLS");
            std::cout << "Enter name: ";
            std::cin >> name;
            points = score;
    }

    void view(){
        std::cout << "NAME: " << name;
        std::cout << ", SCORE: " << points << endl;
    }

    int get_score(){
        return points;
    }

    char* get_name(){
        return name;
    }
};

//to store details of a player into a text file as well as a binary file
void store()
{
    ofstream fo;
    ofstream fto;
    ifstream fi;
    ifstream fti;
    bool flag=true;

    fo.open("temp.dat",ios::binary);
    fi.open("LEADERBOARD.dat",ios::binary);
    fto.open("temp.txt",ios::out);
    fti.open("LEADERBOARD.txt",ios::in);

    player p,temp;
    p.input(level);

    //process to add the player and their score in a sorted manner to both the binary and text files
    while(fi.read((char*)&temp,sizeof(temp)))
    {
        //finding position where new player will be insertes
        if(p.get_score()>temp.get_score() && flag)
        {
            flag=false;
            fo.write((char*)&p,sizeof(p));
            fto << "NAME: " << p.get_name() << ", SCORE: " << p.get_score() << '\n';
        }
        fo.write((char*)&temp,sizeof(temp));
        fto << "NAME: " << temp.get_name() << ", SCORE: " << temp.get_score() << '\n';
    }

    if(flag)
        fo.write((char*)&p,sizeof(p));

    fi.close();
    fo.close();
    fti.close();
    fto.close();

    remove("LEADERBOARD.dat");
    remove("LEADERBOARD.txt");
    rename("temp.dat","LEADERBOARD.dat");
    rename("temp.txt","LEADERBOARD.txt");
}

//To display the leaderboard from the binary file
void show()
{
    ifstream f;
    player p;
    f.open("LEADERBOARD.dat",ios::in);
    system("CLS");
    cout << "LEADERBOARD: " << endl << endl;

    //for case of empty file
    if(!f)
        cout << "EMPTY" << endl;;

    while(f.read((char*)&p,sizeof(player)))
                p.view();
}

//Main parent class for every movinf object in the game
class MovingObject : public Sprite
{
  vector<Sprite*> parts;
  double vx, vy;
  double ax, ay;
  bool paused;

  void initMO(double argvx, double argvy, double argax, double argay, bool argpaused=true)
  {
    vx=argvx; vy=argvy; ax=argax; ay=argay; paused=argpaused;
  }

 public:

 MovingObject(double argvx, double argvy, double argax, double argay, bool argpaused=true): Sprite()
  {
    initMO(argvx, argvy, argax, argay, argpaused);
  }

 MovingObject(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : Sprite()
  {
   double angle_rad = angle_deg*PI/180.0;
   double argvx = speed*cos(angle_rad);
   double argvy = -speed*sin(angle_rad);
   initMO(argvx, argvy, argax, argay, argpaused);
  }

  void set_vx(double argvx)
  {
    vx = argvx;
  }

  void set_vy(double argvy)
  {
    vy = argvy;
  }

  void set_ax(double argax)
  {
    ax = argax;
  }

  void set_ay(double argay)
  {
    ay = argay;
  }

  double getXPos();
  double getYPos();
  void reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta);

  void pause() { paused = true; }
  void unpause() { paused = false; }
  bool isPaused() { return paused; }

  void addPart(Sprite* p) {
    parts.push_back(p);
  }

  void nextStep(double t);
  void getAttachedTo(MovingObject *m);
};

#endif

//MovingObject.cpp

void MovingObject::nextStep(double t)
{
  if(paused)
    return;
  //cerr << "x=" << getXPos() << ",y=" << getYPos() << endl;
  //cerr << "vx=" << vx << ",vy=" << vy << endl;
  //cerr << "ax=" << ax << ",ay=" << ay << endl;

  for(size_t i=0; i<parts.size(); i++)
    parts[i]->move(vx*t, vy*t);

  vx += ax*t;
  vy += ay*t;
} // End MovingObject::nextStep()

double MovingObject::getXPos() {
  return (parts.size() > 0) ? parts[0]->getX() : -1;
}

double MovingObject::getYPos() {
  return (parts.size() > 0) ? parts[0]->getY() : -1;
}

void MovingObject::reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) {
  for(size_t i=0; i<parts.size(); i++)
    parts[i]->moveTo(argx, argy);

  double angle_rad = angle_deg*PI/180.0;
  double argvx = speed*cos(angle_rad);
  double argvy = -speed*sin(angle_rad);
  vx = argvx; vy = argvy; ax = argax; ay = argay; paused = argpaused;
} // End MovingObject::reset_all()

void MovingObject::getAttachedTo(MovingObject *m) {
  double xpos = m->getXPos();
  double ypos = m->getYPos();
  for(size_t i=0; i<parts.size(); i++){
    parts[i]->moveTo(xpos, ypos);
  }
  initMO(m->vx, m->vy, m->ax, m->ay, m->paused);
}

//coin.h
#ifndef __COIN_H__
#define __COIN_H__

//class to descibe all attributes and fucntions related to the bomb in the game
class Bomb : public MovingObject
{
  double bomb_start_x;
  double bomb_start_y;
  double release_speed;
  double release_angle_deg;
  double bomb_ax;
  double bomb_ay;

  // Moving parts
  Circle bomb_circle;

 public:

 Bomb(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta)
 {
    release_speed = speed;
    release_angle_deg = angle_deg;
    bomb_ax = argax;
    bomb_ay = argay;
    initBomb();
  }

  void initBomb();
  void resetBomb();

}; // End class Bomb

//class to describe all attributes and functions related to the coin in the game
class Coin : public MovingObject
{
  double coin_start_x;
  double coin_start_y;
  double release_speed;
  double release_angle_deg;
  double coin_ax;
  double coin_ay;

  // Moving parts
  Circle coin_circle;

 public:

 Coin(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta)
 {
    release_speed = speed;
    release_angle_deg = angle_deg;
    coin_ax = argax;
    coin_ay = argay;
    initCoin();
  }

  void initCoin();
  void resetCoin();

}; // End class Coin

#endif

//lasso.h
#ifndef __LASSO_H__
#define __LASSO_H__

//#define WINDOW_X 1200
//#define WINDOW_Y 960
#define WINDOW_X 800
#define WINDOW_Y 600

#define STEP_TIME 0.05

#define PLAY_X_START 100
#define PLAY_Y_START 0
#define PLAY_X_WIDTH (WINDOW_X-PLAY_X_START)
#define PLAY_Y_HEIGHT (WINDOW_Y-100)

#define LASSO_X_OFFSET 100
#define LASSO_Y_HEIGHT 100
#define LASSO_BAND_LENGTH LASSO_X_OFFSET
#define LASSO_THICKNESS 5

#define COIN_GAP 1

#define RELEASE_ANGLE_STEP_DEG 5
#define MIN_RELEASE_ANGLE_DEG 0
#define MAX_RELEASE_ANGLE_DEG (360-RELEASE_ANGLE_STEP_DEG)
#define INIT_RELEASE_ANGLE_DEG 45

#define RELEASE_SPEED_STEP 20
#define MIN_RELEASE_SPEED 0
#define MAX_RELEASE_SPEED 200
#define INIT_RELEASE_SPEED 100

#define COIN_SPEED 120
#define COIN_ANGLE_DEG 90

#define LASSO_G 30
#define COIN_G 30
#define BOMB_G 20

#define LASSO_SIZE 10
#define LASSO_RADIUS 20
#define COIN_SIZE 5

//function to decide position of coin for each level
int position_of_coin(int level)
{
    if(level==1)
        return (PLAY_X_START+WINDOW_X)/2;
    else if(level==2 || level==4 || level==5)
        return (PLAY_X_START*2 + WINDOW_X)/3;
    else if(level==3)
         return (PLAY_X_START+2*WINDOW_X)/3;
    else
        return (PLAY_X_START);
}

//fucntion to decide position of bomb for each level
int position_of_bomb(int level)
{
    if(level==3 )
        return (PLAY_X_START+WINDOW_X)/2;
    else if(level==4 || level==5)
         return (PLAY_X_START+400);
    else
        return (PLAY_X_START + 100);
}


//main class which consists of the lasso and its various components and functions
class Lasso : public MovingObject
{
  double lasso_start_x;
  double lasso_start_y;
  double release_speed;
  double release_angle_deg;
  double lasso_ax;
  double lasso_ay;

  // Moving parts
  Circle lasso_circle;
  Circle lasso_loop;

  // Non-moving parts
  Line lasso_line;
  Line lasso_band;

  // State info
  bool lasso_looped;

  void initLasso();

 public:

  Coin *the_coin;
  Bomb *the_bomb;

  int num_coins;
  int bomb_hit;

 Lasso(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta)
  {
    release_speed = speed;
    release_angle_deg = angle_deg;
    lasso_ax = argax;
    lasso_ay = argay;
    initLasso();
  }

  void draw_lasso_band();
  void yank();
  void loopit();
  void addAngle(double angle_deg);
  void addSpeed(double speed);

  void nextStep(double t);
  void check_for_coin(Coin *coin);
  void check_for_bomb(Bomb *bomb);

  int getNumCoins()
  {
   return num_coins;
  }

  //to determine if the lasso hits the bomb
  bool ishit()
  {
    if(bomb_hit==1)
        return true;
    else
        return false;
  }

}; // End class Lasso

#endif

//coin.h

//To initialise the coin
void Coin::initCoin()
{
  coin_start_x = position_of_coin(level);
  coin_start_y = PLAY_Y_HEIGHT;
  coin_circle.reset(coin_start_x, coin_start_y, COIN_SIZE);
  coin_circle.setColor(COLOR("yellow"));
  coin_circle.setFill(true);
  addPart(&coin_circle);
}

//setting conditions of the coin after each level/attempt
void Coin::resetCoin()
{
  coin_start_x = position_of_coin(level);
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;

  if(level==2)
    coin_angle_deg = 60;

  if(level==4 || level==5)
  {
    coin_angle_deg = 75;
    coin_speed = COIN_SPEED+30;
  }

  coin_ax = 0;
  coin_ay = COIN_G;
  bool paused = true, rtheta = true;
  reset_all(coin_start_x, coin_start_y, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
}

//to initialise the bomb
void Bomb::initBomb()
{
 // To make the bomb visible only after level 2
  if(level>2)
    bomb_circle.setColor(COLOR("black"));
  else
    bomb_circle.setColor(COLOR("green"));

  bomb_start_x = position_of_bomb(level);
  bomb_start_y = PLAY_Y_HEIGHT-200;
  bomb_circle.reset(bomb_start_x, bomb_start_y, COIN_SIZE);
  bomb_circle.setFill(true);
  addPart(&bomb_circle);
}

//setting conditions of the bomb after each level/attempt
void Bomb::resetBomb()
{
    // To make the bomb visible only after level 2
  if(level>2)
    bomb_circle.setColor(COLOR("black"));
  else
    bomb_circle.setColor(COLOR("green"));

  bomb_start_x = position_of_bomb(level);
  double bomb_speed;

  if(level==5)
    bomb_speed = COIN_SPEED;
  else
    bomb_speed=0;

  if(level==5)
        bomb_start_y = PLAY_Y_HEIGHT;
  else
        bomb_start_y = PLAY_Y_HEIGHT-200;

  double bomb_angle_deg = COIN_ANGLE_DEG;
  bomb_ax = 0;
  bomb_ay = BOMB_G;
  bool paused = true, rtheta = true;
  reset_all(bomb_start_x, bomb_start_y, bomb_speed, bomb_angle_deg, bomb_ax, bomb_ay, paused, rtheta);
}

//lasso.cpp

void Lasso::draw_lasso_band()
{
  double len = (release_speed/MAX_RELEASE_SPEED)*LASSO_BAND_LENGTH;
  double arad = release_angle_deg*PI/180.0;
  double xlen = len*cos(arad);
  double ylen = len*sin(arad);
  lasso_band.reset(lasso_start_x, lasso_start_y, (lasso_start_x-xlen), (lasso_start_y+ylen));
  lasso_band.setThickness(LASSO_THICKNESS);
} // End Lasso::draw_lasso_band()

//to initialise conditions of the lasso
void Lasso::initLasso()
{
  lasso_start_x = (PLAY_X_START+LASSO_X_OFFSET);
  lasso_start_y = (PLAY_Y_HEIGHT-LASSO_Y_HEIGHT);

  lasso_circle.reset(lasso_start_x, lasso_start_y, LASSO_SIZE);
  lasso_circle.setColor(COLOR("red"));
  lasso_circle.setFill(true);

  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setColor(COLOR("red"));
  lasso_loop.setFill(true);

  addPart(&lasso_circle);
  addPart(&lasso_loop);

  lasso_looped = false;
  the_coin = NULL;
  num_coins = 0;
  the_bomb = NULL;
  bomb_hit=0;

  lasso_line.reset(lasso_start_x, lasso_start_y, lasso_start_x, lasso_start_y);
  lasso_line.setColor(COLOR("red"));

  lasso_band.setColor(COLOR("blue"));
  draw_lasso_band();

} // End Lasso::initLasso()

void Lasso::yank()
{
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);

  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setFill(true);
  lasso_looped = false;
  yanked =true;

  //if the coin is caught
  if(the_coin != NULL) {
    num_coins++;
    the_coin->resetCoin();
  }

} // End Lasso::yank()

void Lasso::loopit()
{
  if(lasso_looped)
   return;  // Already looped

  lasso_loop.reset(getXPos(), getYPos(), LASSO_RADIUS);
  lasso_loop.setFill(false);
  lasso_looped = true;
} // End Lasso::loopit()

//To edit the angle of projection of lasso during the game
void Lasso::addAngle(double angle_deg)
{
  release_angle_deg += angle_deg;

  if(release_angle_deg < MIN_RELEASE_ANGLE_DEG)
   release_angle_deg = MIN_RELEASE_ANGLE_DEG;

  if(release_angle_deg > MAX_RELEASE_ANGLE_DEG)
   release_angle_deg = MAX_RELEASE_ANGLE_DEG;

  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addAngle()

//To edit the spped of release of lasso during the game
void Lasso::addSpeed(double speed)
{
  release_speed += speed;

  if(release_speed < MIN_RELEASE_SPEED)
    release_speed = MIN_RELEASE_SPEED;

  if(release_speed > MAX_RELEASE_SPEED)
    release_speed = MAX_RELEASE_SPEED;

  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addSpeed()

//To make changes after each time interval
void Lasso::nextStep(double stepTime)
{
  draw_lasso_band();
  MovingObject::nextStep(stepTime);
  if(getYPos() > PLAY_Y_HEIGHT)
   yank();
  lasso_line.reset(lasso_start_x, lasso_start_y, getXPos(), getYPos());
} // End Lasso::nextStep()

//to check whether the lasso has caught the coin
void Lasso::check_for_coin(Coin *coinPtr)
{
  double lasso_x = getXPos();
  double lasso_y = getYPos();
  double coin_x = coinPtr->getXPos();
  double coin_y = coinPtr->getYPos();
  double xdiff = (lasso_x - coin_x);
  double ydiff = (lasso_y - coin_y);
  double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));

  if(distance <= LASSO_RADIUS) {
    the_coin = coinPtr;
    the_coin->getAttachedTo(this);
  }
} // End Lasso::check_for_coin()

//to check whether the lasso hit any bomb on its way
void Lasso::check_for_bomb(Bomb *bombPtr)
{
  double lasso_x = getXPos();
  double lasso_y = getYPos();
  double bomb_x = bombPtr->getXPos();
  double bomb_y = bombPtr->getYPos();
  double xdiff = (lasso_x - bomb_x);
  double ydiff = (lasso_y - bomb_y);
  double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));

  if(distance <= LASSO_RADIUS)
  {
    got_hit++;
    bomb_hit=1;
    level--;
  }
} // End Lasso::check_for_bomb()

//To display the health bar which shows the number of lives remaining
void draw_health(int lives,Circle c[])
{
    for(int i=0;i<3;i++)
    {
        if(i<lives)
            c[i].setColor(COLOR("red"));
        else
            c[i].setColor(COLOR("green"));
    }
}


main_program {
  //Initialising the main canvas i.e game window and required variables
  initCanvas("Lasso", WINDOW_X, WINDOW_Y);
  Rectangle screen(400,300,WINDOW_X,WINDOW_Y);
  screen.setColor(COLOR("green"));
  screen.setFill(true);
  int stepCount = 0;
  float stepTime = STEP_TIME;
  float runTime = -1; // sec; -ve means infinite
  float currTime = 0;

  // Draw lasso at start position
  double release_speed = INIT_RELEASE_SPEED; // m/s
  double release_angle_deg = INIT_RELEASE_ANGLE_DEG; // degrees
  double lasso_ax = 0;
  double lasso_ay = LASSO_G;
  bool paused = true;
  bool rtheta = true;

  //To initialise the lasso with default parameters
  Lasso lasso(release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);

  //To show the axes i.e the main window of the game
  Line b1(PLAY_X_START, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
  b1.setColor(COLOR("blue"));
  Line b2(PLAY_X_START, PLAY_Y_START, PLAY_X_START, WINDOW_Y-100);
  b2.setColor(COLOR("blue"));

  //to display which command is being called
  string msg("Cmd: _");
  Text charPressed(PLAY_X_START+50, PLAY_Y_HEIGHT+20, msg);

  //to display level of game and health bar
  char coinScoreStr[256],health[200];
  int lives=3;

  sprintf(coinScoreStr, "Level: %d", level);
  sprintf(health, "Lives remaining: ");
  Text coinScore(PLAY_X_START+100, PLAY_Y_HEIGHT+50, coinScoreStr);
  Text lives_left(PLAY_X_START+80, PLAY_Y_HEIGHT+80,health);

  //To display the timer during each level
  char time_left[100];
  sprintf(time_left,"Time remaining: %d",timer);
  Text time(PLAY_X_START+500,20,time_left);

  //For displaying heath levels initially
  Circle health_circles[3];
  for(int i=0;i<3;i++)
  {
        health_circles[i].reset(PLAY_X_START+140+12.5*i,PLAY_Y_HEIGHT+80,5);
        health_circles[i].setColor(COLOR("red"));
        health_circles[i].setFill(true);
  }

  //To initialise coin and its default parameters
  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;
  Coin coin(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);

  //To initialise bomb and its default parameters
  double bomb_speed = 0;
  double bomb_angle_deg = COIN_ANGLE_DEG;
  double bomb_ax = 0;
  double bomb_ay = COIN_G;
  Bomb bomb(bomb_speed, bomb_angle_deg, bomb_ax, bomb_ay, paused, rtheta);

  // After every COIN_GAP sec, make the object(bomb/coin) jump
  double last_coin_jump_end = 0;
  double last_bomb_jump_end = 0;

  //initial node in the llinked list which stores 0 score as acts as the last link
  scores *temp_score= new scores();;
  temp_score->score=0;
  temp_score->next=NULL;

  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When c is pressed, loop lasso
  // When q is pressed, quit
  // When l is pressed, show leaderboard
  // When [ or ] is pressed, change angle of projection
  // When - is pressed decrease speed of projection
  // When = is pressed increase speed of projection

  for(;;) {

    //in case game crashes and doesnt run properly
    if((runTime > 0) && (currTime > runTime))
        break;

    //For taking a character as input for each command
    XEvent e;
    bool pendingEv = checkEvent(e);

    if(pendingEv) {

      char c = charFromEvent(e);
      msg[msg.length()-1] = c;

      //To display the command that is being called
      charPressed.setMessage(msg);

      switch(c)
      {

      case 't': //throw
        lasso.unpause();
        break;

      case 'y': //yank
        lasso.yank();
        break;

      case 'c': //catch coin/loop lasso
        lasso.loopit();
        lasso.check_for_coin(&coin);
        wait(STEP_TIME*5);
        break;

      case '[': //change angle of projection
        if(lasso.isPaused())
        {
            lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);
        }
        break;

      case ']': //change angle of projection
        if(lasso.isPaused())
            lasso.addAngle(+RELEASE_ANGLE_STEP_DEG);
        break;

      case '-': // decrease speed of projection
        if(lasso.isPaused())
            lasso.addSpeed(-RELEASE_SPEED_STEP);
        break;

      case '=': //increase speed of projection
        if(lasso.isPaused())
         lasso.addSpeed(+RELEASE_SPEED_STEP);
        break;

      //To quit game midway
      case 'q':
        sprintf(coinScoreStr, "GAME OVER, You have completed %d level(s),Total score: %d",level-1,score);
        coinScore.setMessage(coinScoreStr);
        store();
        wait(3);
        exit(0);

       // To display leaderboard midway in the game
      case 'l':
            show();
            break;

      default:
        break;
      }
    }

    coin.nextStep(stepTime);

    //For level 5, the bomb is also moving, so these are the required settings for that
    if(level==5)
    {
        bomb.nextStep(stepTime);
        if(bomb.isPaused())
        {
            if((currTime-last_bomb_jump_end) >= COIN_GAP)
                bomb.unpause();
        }
    }

    //Conditiond to ensure coin is moving properly
    if(coin.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
            coin.unpause();
      }
    }

    lasso.nextStep(stepTime);

    //for levels higher than 2, bombs will be present
    if(level>2)
        lasso.check_for_bomb(&bomb);

    //when lasso is reset to its original position
    if(yanked)
    {
        if(lasso.getNumCoins()==0)
            lives--;
    // to update health bar
        draw_health(lives,health_circles);

      //If all lives are over
        if(!lives)
        {
            sprintf(coinScoreStr, "GAME OVER, You have completed %d level(s),Total score: %d",level-1,score);
            coinScore.setMessage(coinScoreStr);
            store();
            exit(0);
        }
      yanked = false;
    }

    // If the lasso hits a bomb, to go back a level
    if(lasso.ishit())
    {
        //Substract all the points earned from the previous level
        score-=temp_score->score;

        //to free up space used by previous pointer i.e dynamic deletion of node
        scores* temp = temp_score;
        temp_score=temp_score->next;
        delete temp;

        //if bomb is hit 3 times, game over
        if(got_hit==3)
        {
            sprintf(coinScoreStr, "Oh no, this is the third time you hit the bomb, GAME OVER");
            coinScore.setMessage(coinScoreStr);
            wait(2);
            sprintf(coinScoreStr, "GAME OVER, You have completed %d level(s),Total score: %d",level-1,score);
            coinScore.setMessage(coinScoreStr);
            store();
            exit(0);
        }

        sprintf(coinScoreStr, "You hit a bomb %d time(s), You go back to level %d",got_hit, level);
        coinScore.setMessage(coinScoreStr);
        lasso.bomb_hit=0;
        wait(3);
        lasso.yank();

        //Resetting everything to the prev level
        bomb.resetBomb();
        coin.resetCoin();
        lives=4;
        draw_health(lives,health_circles);
        currTime=0.0;

        sprintf(coinScoreStr, "Level: %d", level);
        coinScore.setMessage(coinScoreStr);
        coin.unpause();
    }

    // to reset(rethrow) coin when it hits the ground
    if(coin.getYPos() > PLAY_Y_HEIGHT) {
      coin.resetCoin();
      last_coin_jump_end = currTime;
    }

    // To rethrow bomb when it hits the ground
    if(level==5){
        if(bomb.getYPos() > PLAY_Y_HEIGHT)
        {
            bomb.resetBomb();
            last_bomb_jump_end = currTime;
        }
    }

    //If coin is caught without hitting any bombs
    if(lasso.getNumCoins())
    {
        //algorith to calculate score after each level using values of time used to complete the level and difficulty of the level
        score += level*timer;

        //To update the linked list with the points earned in the last level
        scores *s = new scores();
        s->score = level*timer;
        s->next = temp_score;
        temp_score = s;

        sprintf(coinScoreStr, "Level %d complete!!!",level++);
        coinScore.setMessage(coinScoreStr);

        //If all 5 levels are completed
        if(level==6)
        {
            sprintf(coinScoreStr, "CONGRATULATIONS, You have completed all the levels. Total score: %d",score);
            coinScore.setMessage(coinScoreStr);
            store();
            exit(0);
        }

        //Resetting lasso conditions
        lasso.the_coin = NULL;
        lasso.num_coins=0;
        wait(2);

        //display new level after 2 seconds
        sprintf(coinScoreStr, "Level: %d", level);
        coinScore.setMessage(coinScoreStr);
        bomb.resetBomb();
        coin.resetCoin();

        lives=3;
        draw_health(lives,health_circles);
        currTime=0.0;
        coin.unpause();
        bomb.unpause();
    }

    //To update timer after each time interval
    stepCount++;
    currTime += stepTime;
    wait(stepTime);
    timer=60-int(currTime);
    sprintf(time_left,"Time remaining: %d",timer);

    time.setMessage(time_left);

    //If time is up for a particular level
    if(timer==0)
    {
            sprintf(coinScoreStr, "GAME OVER, You have completed %d level(s),Total score: %d",level-1,score);
            coinScore.setMessage(coinScoreStr);
            store();
            exit(0);
    }
  } // End for(;;)
  wait(3);
} // End main_program
