


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
  // Set alarm to call onTimer function every 100 millisecond (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 100000, true);
  // Start an alarm
  timerAlarmEnable(timer);
  pinMode(POWERSWITCH,INPUT_PULLUP);
  Serial.begin(115200);    
  espSerial.begin("pump1");                     // function in virtuino.h
  vbt.begin(onReceived,onRequested,256);        // function in virtuino.h
  Serial.println("Setup completed.");
  pump1.initialize(19,21,22);                   // function in pump_functions.h
  currentState = pump1.pumpState();             // function in pump_functions.h
  Serial.println(currentState);
  default_setup();
  StartStop = false;
  attachInterrupt(digitalPinToInterrupt(POWERSWITCH), switchIsr, FALLING);
}

void loop() {
  virtuinoRun(); // function in virtuino.h - neccesary command to communicate with Virtuino android app
  while (Serial.available()) 
   { 
    serialRun();
   }
  if(!StartStop)
   {
        return;
   }

  if (((runType == Cycles) && (cycles > 0)) || runType == Continuous)
   {
    if ((runTime <= 0)||(currentState != pump1.pumpState()) )
     {
       goNextState();
       pump1.run(dir,dir1,pumpSpeedCurrent,currentState);   // function in pump_functions.h
       currentState = pump1.pumpState();       // function in pump_functions.h
       Serial.println(currentState);
     }
 
   }
};

void goNextState()
{  
  switch (currentState)
  {
     case PowerOn:
      currentState = Idle;
      runTime = 0;
      break;
    
    case Stop:
      currentState = PowerOn;
      pumpSpeedCurrent = 0;
      runTime = 0;
      break;
      
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
      runTime = runTime3;
      pumpSpeedCurrent = 0;      
      break;
   
    case Hold:
       if(runType == Cycles){
         cycles--;
         Serial.println( cycles*10); 
         if(cycles <= 0){
             startstop(false);
          }
         }
         if ( runType != Cycles || cycles >0){
             currentState = RunFirst;
             pumpSpeedCurrent = pumpSpeedRun;
             runTime = runTime1;
          }
     
      break;  
    
    default:
      currentState = Stop;
  }
  Serial.println(runTime);
};

void default_setup()
{
  // Start of dummy variables
  dir = HIGH;
  dir1 = LOW;
  pumpSpeedRun = 200;
  pumpSpeedSuck = 255;
  pumpSpeedHold = 5;
  runType = Continuous;
  reload_cycles = cycles = 5;
  runTime = 10;
  runTime1 = 110;
  runTime2 = 120;
  runTime3 = 130;
  // end of dummy variables
};

void startstop(bool sw)

{
  if(sw)
  {
    currentState = PowerOn;
    StartStop = true;       //start pump
    runTime = 0;
    cycles = reload_cycles;
    pump1.run(HIGH,HIGH,pumpSpeedCurrent,currentState);
    Serial.println("Start");
  }
  else
  {
    currentState = Stop;
    StartStop = false; //stop pump
    runTime = 0;
   pump1.run(HIGH,HIGH,pumpSpeedCurrent,currentState);
    Serial.println("Stop");
  }
}

void switchIsr() // when the start/Stop switch is operated. 
{

  //main function is to initiate or stop pump actions by setting the pump state and initiating the internal timer
  if (StartStop)            //running
  {
        startstop(false);
            
  }

 else                       //not running 
 {
    startstop(true);
 }
}
