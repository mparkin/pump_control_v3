#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#include "pump_state_machine.h"

class pumpcontrols : public pumpState
{
  private:
     pumpState pState;
     int cycles, rpm;
     bool rotate, rdirection;
     float seconds;
     bool changeState(state nState);
  public:
     pumpcontrols(){pState.newState(PowerOff);}
     ~pumpcontrols(){pState.newState(PowerOff);}
     void initialize(bool idir1,bool idir2, int ispeed);
     void powerdown();
     void run(bool dir,bool dir1,unsigned int speed);
     void runTimed(float seconds);
};

void pumpcontrols::initialize(bool idir1,bool idir2, int ispeed)
{
  pState.newState(Idle);
  if (pState.current() == Idle)
  {
     rotate = idir1;
     rdirection = idir2;
     rpm = ispeed;
     pinMode(rpm, OUTPUT); //motor speed PWM
     pinMode(rotate, OUTPUT); //                       
     pinMode(rdirection, OUTPUT); //
  }
};

void pumpcontrols::powerdown()
{
  if (changeState(PowerOff))
  {
//    analogWrite(rpm,0);
    digitalWrite(rotate,LOW);
    digitalWrite(rdirection,LOW);
    pState.newState(PowerOff);     
  }
};

void pumpcontrols::run(bool dir,bool dir1,unsigned int speed)
{
  if (pState.current() == Idle|pState.current() == Hold) 
  {
//    analogWrite(rpm,speed);
    digitalWrite(rotate,dir);  //need to set level programmatically and not hard coded
    digitalWrite(rdirection,dir1);
    pState.newState(RunFirst); 
  }
  else if (pState.current() == RunFirst)
  {
//    analogWrite(rpm,speed);
    digitalWrite(rotate,dir); //need to set level programmatically and not hard coded
    digitalWrite(rdirection,dir1);
    pState.newState(RunSecond);
  }
  else if (pState.current() == RunSecond)
  {
//    analogWrite(rpm,speed);
    digitalWrite(rotate,LOW);
    digitalWrite(rdirection,LOW);
    pState.newState(Hold);
  }
  else pState.newState(Idle);
};

void pumpcontrols::runTimed(float seconds)
{
  
};

bool pumpcontrols::changeState(state nState)
{
  pState.newState(nState);
  if (pState.current() == nState)
    return true;
  else return false;
};
