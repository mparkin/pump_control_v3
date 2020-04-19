

enum Mode
{
  Off,
  Timed,
  Cycles,
  Continuous
};


#define POWERSWITCH 23

int counterDemo=0;

volatile uint32_t runTime = 0;
volatile uint32_t runTime1,runTime2,runTime3;       
static unsigned int   pumpSpeedCurrent,pumpSpeedRun,pumpSpeedSuck,pumpSpeedHold;
int temperature;
int humidity;
float DHTperiod;
unsigned int cycles,reload_cycles;
bool StartStop, dir,dir1;
Mode runType, opstate;      
String readBuffer;
