/*   Requirements
//   Virtual Switch  for Start/Stop
//   Start/Stop  sw  pin8 affects same boolean as Virt Start/Stop
//   Virtual Sw EMO  hw PIN9 boolean
//   EMO pin9 
//   Analog input pin 6 Temp/Humidity  DHT11
//                         4,7
                    //     L,L stop
                    //     H,L cw
                    //     L,H ccw  L298 driver
// Control Registers 
     Pump On Time & speed(0-255)
     Pump Hold Time & speed 
     Pump SuckbackTime & speed
     Pump Wait
     cycle counter
     temperature
     Humidity
     DHT11 cycle time
       
*/

#include "pump_functions.h"
//#include "VirtuinoBluetooth.h"                   
#include <SoftwareSerial.h>            
#include "BluetoothSerial.h"
#include "VirtuinoCM.h" 

#define POWERSWITCH 32
BluetoothSerial espSerial;

VirtuinoCM vbt;
#define V_memory_count 32          // the size of V memory. You can change it to a number <=255)
float V[V_memory_count];           // This array is synchronized with Virtuino V memory. You can change the type to int, long etc.

boolean debug = true;              // set this variable to false on the finale code to decrease the request time.

float V1_lastValue=0;
int counterDemo=0;

//SoftwareSerial bluetoothSerial =  SoftwareSerial(10,11);   
//VirtuinoBluetooth virtuino(bluetoothSerial,9600); 
pumpcontrols pump1;

float runTime1, runTime2, holdTime,runTime;       
unsigned int   pumpSpeedCurrent,pumpSpeedRun,pumpSpeedSuck,pumpSpeedHold;
int temperature;
int humidity;
float DHTperiod;
unsigned int cycles;
bool StartStop, runType,dir,dir1;
state currentState;               

void setup() {
  cli(); //this shuts down interrupts and allows setup to initialize and configure the timers needed in loop.


  Serial.begin(115200);    
  espSerial.begin("pump1");
  vbt.begin(onReceived,onRequested,256);
  Serial.println("Setup completed.");
  pump1.initialize(19,21,22);
  currentState = pump1.pumpState();
  Serial.println(currentState);
  attachInterrupt(digitalPinToInterrupt(POWERSWITCH), switchIsr, RISING);
  sei();//allow interrupts again after setting everything up
}

void loop() {
  virtuinoRun();                        //  neccesary command to communicate with Virtuino android app
  dir = HIGH;
  dir1 = LOW;
  pumpSpeedRun = 200;
  pumpSpeedSuck = 255;
  pumpSpeedHold = 5;
  cycles = 5;
  // end of dummy variables
  if ( currentState != pump1.pumpState()) //the start stop interrupt will change pump1.,current to runFirst to start this
  {    switch(pump1.pumpState())           // the pump will run until the duration timer runs to zero then an ISR will change
       {
           case RunFirst:
              pumpSpeedCurrent = pumpSpeedRun;
              break;
           case RunSecond:
              pumpSpeedCurrent = pumpSpeedSuck;
              break;
           default:
               pumpSpeedCurrent = 0;
             
       }
       pump1.run(dir,dir1,pumpSpeedCurrent);   // pump1 state to runSecond
       currentState = pump1.pumpState();
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
      runTime = runTime1;
    case RunFirst:
      currentState = RunSecond;
      runTime = runTime2;      
    case RunSecond:
      currentState = Hold;
      runTime = holdTime
      
      ;      
    default:
      currentState = Stop;
  }
}


void TimerIsr() //this is the function that occurs when the hardware interval timer hits zero
{
  //main function is to reload the timer conpare register, change the pump state and then restart the timer.
  // the timer trimesout every 1 second. Then it decrements the runTime counter. 
  // When runTime counter == 0, it sets the run state to next value.
  runTime--; 
  if (runTime <= 0 ) goNextState();
};

void switchIsr() // when the staurt/Stop switch is operated. 
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

//============================================================== onCommandReceived
//==============================================================
/* This function is called every time Virtuino app sends a request to server to change a Pin value
 * The 'variableType' can be a character like V, T, O  V=Virtual pin  T=Text Pin    O=PWM Pin 
 * The 'variableIndex' is the pin number index of Virtuino app
 * The 'valueAsText' is the value that has sent from the app   */
 void onReceived(char variableType, uint8_t variableIndex, String valueAsText){     
    if (variableType=='V'){
        float value = valueAsText.toFloat();        // convert the value to float. The valueAsText have to be numerical
        if (variableIndex<V_memory_count) V[variableIndex]=value;              // copy the received value to arduino V memory array
    }
}

//==============================================================
/* This function is called every time Virtuino app requests to read a pin value*/
String onRequested(char variableType, uint8_t variableIndex){     
    if (variableType=='V') {
    if (variableIndex<V_memory_count) return  String(V[variableIndex]);   // return the value of the arduino V memory array
  }
  return "";
}

//============================================================== virtuinoRun
  void virtuinoRun(){
    while (espSerial.available()) {
        char tempChar=espSerial.read();
        if (tempChar==CM_START_CHAR) {               // a new command is starting...
              vbt.readBuffer=CM_START_CHAR;     // copy the new command to the virtuino readBuffer
              vbt.readBuffer+=espSerial.readStringUntil(CM_END_CHAR);
              vbt.readBuffer+=CM_END_CHAR;
              if (debug) Serial.println("\nCommand= "+vbt.readBuffer);
              String* response= vbt.getResponse();    // get the text that has to be sent to Virtuino as reply. The library will check the inptuBuffer and it will create the response text
              if (debug) Serial.println("Response : "+*response);
              espSerial.print(*response);
              break; 
         }
    }
 }


 //============================================================== vDelay
  void vDelay(int delayInMillis){long t=millis()+delayInMillis;while (millis()<t) virtuinoRun();}
