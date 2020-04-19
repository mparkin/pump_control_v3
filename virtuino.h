/*
 * 
 * Vituino array allocations
 *  V5 = Digital Start / Stop Switch
 *  T0 = Run Mode [ 'Continuous','Cycles','Timed']
 *  V0 = Phase one runtime 
 *  V1 = Phase two runtime
 *  V2 = Phase three runtime
 *  V3 = Number of cycles to run
 *  V4  = Time to run
 *  T1 = direction ['Forward,'Reverse','Hold'
 *  T2 = Status ['Running','Stopped','Error']
 *  A0 = Phase one speed
 *  A1 = phase two speed
 *  A3 = phase three speed
 *  #define CM_WELCOME_MESSAGE  "!C=VirtuinoCM 1.0.02" 
 *  #define CM_START_CHAR '!'                 // All Virtuino commands starts with !
 *  #define CM_END_CHAR   '$'                 // All Virtuino commands ends with $ 
 *  #define CM_ERROR  "E00=7$"           
 *  #define CM_ERROR_KEY "E00=5$"     
 *  get the type of command A,V,Q,O,T,C 
 */


#include "BluetoothSerial.h"
#include "VirtuinoCM.h" 


void startstop(bool sw);

BluetoothSerial espSerial;
VirtuinoCM vbt;

#define V_memory_count 32          // the size of V memory. You can change it to a number <=255)
float V[V_memory_count];           // This array is synchronized with Virtuino V memory. You can change the type to int, long etc.
float V1_lastValue=0;

boolean debug = true;              // set this variable to false on the finale code to decrease the request time.

//============================================================== onCommandReceived
//==============================================================
/* This function is called every time Virtuino app sends a request to server to change a Pin value
 * The 'variableType' can be a character like V, T, O  V=Virtual pin  T=Text Pin    O=PWM Pin 
 * The 'variableIndex' is the pin number index of Virtuino app
 * The 'valueAsText' is tpehe value that has sent from the app   */
 
 void onReceived(char variableType, uint8_t variableIndex, String valueAsText){   
    if (variableType=='V'){
      switch(variableIndex)
      {
        case 0:
           break;
        case 1:
           break;
        case 2:
           break;
        case 3:
           cycles = valueAsText.toInt();
           Serial.println( cycles);
           break; 
        case 4:
           break;
        case 5:
           if(valueAsText.toInt()==1){
             startstop(true);
            }
           else
             startstop(false);
             break;                      
     }
    }
    else if (variableType=='A'){
      if (variableIndex==0) {pumpSpeedRun = valueAsText.toInt();}           //convert to int and set Phase One Speed      
      else if (variableIndex==0) {pumpSpeedSuck = valueAsText.toInt();}      //convert to int and set Phase Two Speed      
      else if (variableIndex==0) {pumpSpeedHold = valueAsText.toInt();}      //convert to int and set Phase Three Speed      
      
    }
    else if (variableType=='Q'){
      
    }
    else if (variableType=='O'){
    }
    else if (variableType=='T'){
      if (variableIndex==0) {
        if(valueAsText == "Continuous")runType = Continuous;
        else if(valueAsText == "Cycles")runType = Cycles ; 
        else if(valueAsText == "Timed")runType = Timed;
        Serial.println (valueAsText);
        Serial.println (runType);
        }         //check and change Mode value ['Continuous','Cycles','Timed']      
      if (variableIndex == 1){}
    }
    else if (variableType=='C'){
      
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
              //vbt.readBuffer+=CM_END_CHAR;
              if (debug) Serial.println("\nCommand= "+vbt.readBuffer);
              String* response= vbt.getResponse();    // get the text that has to be sent to Virtuino as reply. The library will check the inptuBuffer and it will create the response text
              if (debug) Serial.println("Response : "+*response);
              espSerial.print(*response);
              break; 
         }
    }
  }
 void serialRun(){

    while (Serial.available()) {
        char tempChar=Serial.read();
        if (tempChar==CM_START_CHAR) {               // a new command is starting...
              readBuffer=CM_START_CHAR;     // copy the new command to the virtuino readBuffer
              readBuffer+=Serial.readStringUntil(CM_END_CHAR);
              //readBuffer+=CM_END_CHAR;
              if (debug) Serial.println("\nCommand= "+readBuffer);
              int delim = readBuffer.indexOf("=");
              onReceived(readBuffer.charAt(1), readBuffer.substring(2,delim).toInt(), readBuffer.substring(delim+1));
              //String* response= vbt.getResponse();    // get the text that has to be sent to Virtuino as reply. The library will check the inptuBuffer and it will create the response text
              //if (debug) Serial.println("Response : "+*response);
              //Serial.print(*response);
              break; 
         }
    }
 }


 //============================================================== vDelay
  void vDelay(int delayInMillis){long t=millis()+delayInMillis;while (millis()<t) virtuinoRun();}
