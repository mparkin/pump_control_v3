#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif
#include "analogWrite.h"
#include "pump_state_machine.h"

class pumpcontrols : public pumpState
{
  private:
     pumpState pState;
     int cycles, rpm;
     int rotate, rdirection;
     float seconds;
     bool changeState(state nState);
  public:
     pumpcontrols(){pState.newState(PowerOff);}
     ~pumpcontrols(){pState.newState(PowerOff);}
     void initialize(int idir1,int idir2, int ispeed);
     void powerdown();
     void run(bool dir,bool dir1,unsigned int speed);
     void runTimed(float seconds);
     state pumpState();
};

void pumpcontrols::initialize(int idir1,int idir2, int ispeed)
{
  pState.newState(PowerOn);
  if(pumpState() == PowerOn)
  {
     rotate = idir1;
     rdirection = idir2;
     rpm = ispeed;
     pinMode(rotate, OUTPUT); //motor speed PWM
     pinMode(rdirection, OUTPUT); //                      
     pinMode(rpm, OUTPUT); //
  }
};

void pumpcontrols::powerdown()
{
  if (changeState(PowerOff))
  {
    analogWrite(rpm,0);
    digitalWrite(rotate,LOW);
    digitalWrite(rdirection,LOW);
    pState.newState(PowerOff);     
  }
};

void pumpcontrols::run(bool dir,bool dir1,unsigned int speed)
{
  if (pState.current() == Idle|pState.current() == Hold) 
  {
    analogWrite(rpm,speed);
    digitalWrite(rotate,dir);  //need to set level programmatically and not hard coded
    digitalWrite(rdirection,dir1);
    pState.newState(RunFirst); 
  }
  else if (pState.current() == RunFirst)
  {
    analogWrite(rpm,speed);
    digitalWrite(rotate,!dir); //need to set level programmatically and not hard coded
    digitalWrite(rdirection,!dir1);
    pState.newState(RunSecond);
  }
  else if (pState.current() == RunSecond)
  {
    analogWrite(rpm,0);
    digitalWrite(rotate,LOW);
    digitalWrite(rdirection,LOW);
    pState.newState(Hold);
  }
  else if (pState.current() == Stop)
  {
    analogWrite(rpm,0);
    digitalWrite(rotate,HIGH);
    digitalWrite(rdirection,HIGH);
    pState.newState(Stop);
  }
  else 
  {
    pState.newState(Idle);
    analogWrite(rpm,0);
    digitalWrite(rotate,LOW);
    digitalWrite(rdirection,LOW);

  }
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

state pumpcontrols::pumpState()
{
  return pState.current();
};
