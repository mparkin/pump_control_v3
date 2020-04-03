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

#define POWERSWITCH 32

int counterDemo=0;


float runTime1, runTime2, holdTime,runTime;       
unsigned int   pumpSpeedCurrent,pumpSpeedRun,pumpSpeedSuck,pumpSpeedHold;
int temperature;
int humidity;
float DHTperiod;
unsigned int cycles;
bool StartStop, runType,dir,dir1;
               
