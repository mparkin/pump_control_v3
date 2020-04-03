volatile int interruptCounter;
int totalInterruptCounter;


void goNextState();
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  runTime--; 
 // if (runTime <= 0 ) goNextState();
  portEXIT_CRITICAL_ISR(&timerMux);
 
}
