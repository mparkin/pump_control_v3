
#ifndef VARS
#include "variables.h"
#define VARS
#endif
#include "pump_functions.h"                 
#include "timer.h"
#include "virtuino.h"


state currentState;
pumpcontrols pump1;


void setup() {
 // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 100000, true);

  // Start an alarm
  timerAlarmEnable(timer);

  Serial.begin(115200);    
  espSerial.begin("pump1");                     // function in virtuino.h
  vbt.begin(onReceived,onRequested,256);        // function in virtuino.h
  Serial.println("Setup completed.");
  pump1.initialize(19,21,22);                   // function in pump_functions.h
  currentState = pump1.pumpState();             // function in pump_functions.h
  Serial.println(currentState);
  default_setup();
  attachInterrupt(digitalPinToInterrupt(POWERSWITCH), switchIsr, RISING);
}

void loop() {
  virtuinoRun();                                // function in virtuino.h - neccesary command to communicate with Virtuino android app
  
  if ((runTime <= 0)||(currentState != pump1.pumpState()) )
  {
       goNextState();
       Serial.println(pumpSpeedCurrent);
       pump1.run(dir,dir1,pumpSpeedCurrent);   // function in pump_functions.h
       currentState = pump1.pumpState();       // function in pump_functions.h
       Serial.println(currentState);
  }
  while (Serial.available()) {
     currentState=(state) Serial.read(); 
  }
};

void goNextState()
{
  
  switch (currentState)
  {
    case Stop:
    case Idle:
      currentState = RunFirst;
      pumpSpeedCurrent = pumpSpeedRun;
      runTime = runTime1;
      break;
   
    case RunFirst:
      currentState = RunSecond;
      pumpSpeedCurrent = pumpSpeedSuck;
      runTime = runTime2;      
      break;
   
    case RunSecond:
      currentState = Hold;
      runTime = holdTime;
      pumpSpeedCurrent = 0;
      break;
   
    case Hold:
      currentState = RunFirst;
      pumpSpeedCurrent = pumpSpeedRun;
      runTime = runTime1;
      break;  
    
    default:
      currentState = Stop;
  }
  pumpSpeedCurrent = pumpSpeedRun;
  Serial.println(pumpSpeedCurrent);
};

void default_setup()
{
  // Start of dummy variables
  dir = HIGH;
  dir1 = LOW;
  pumpSpeedRun = 200;
  pumpSpeedSuck = 255;
  pumpSpeedHold = 5;
  cycles = 5;
  runTime = 100;
  // end of dummy variables
};

void switchIsr() // when the start/Stop switch is operated. 
{
  //main function is to initiate or stop pump actions by setting the pump state and initiating the internal timer
  if (StartStop)            //running
  {
    currentState = Stop;
    StartStop = false;     //stop pump
  }

 else                       //not running 
 {
    currentState = RunFirst;
    StartStop = true;       //start pump
 }
}
