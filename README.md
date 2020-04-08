# pump_control_v3
>>>>>>> c162f080b3174723bbd0df45bbd2320b43ce1bc3
## Customer Code to control pumps. Based on Heltec wifi kit ESP32 dev board

## Main fetures
. Pump Control lines:
   .Digital oupout pins 
      . Controls direction
      . controls movement
   . Analog output pin
      . 0-255 control of pump speed
. 3 operation modes
   .continuous
   .Cycles
   .timed

. 3 sub operations 
   .Phase 1
   .Phase 2
   .Phase 3

. Virtuino HID
. Serial monitor port inteface


## Communications Protocol

## Based ob Virtuino message protocol of command dlimiter (!), Type od command (A,T,V.0,Q,C) and eod command ($)

To start pump operation : !V5=1$
To stop pump operation : !V5=0$

Other commands will be added as to the README.mf file when verified as working. 
