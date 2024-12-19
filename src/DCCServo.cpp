/* 
ServoDriverMulti

This sketch allows an Arduino Nano to operate a number of servos 
for railway point control.

Operating paramaters are programmable and stored in the EEPROM

*/
#include <Arduino.h>
//#include "avr8-stub.h"

#include "DCCServo.h"

int STARTADDRESS = 200; // Set this value to the start address
//bool ADDRESSRECEIVED = false; // indicates a valid address interpreted

//bool SetupMode = false;   // Indicates the current setup state
//bool buttonloop = false;

bool SetupMode = false;   // Indicates the current setup state
//int currentpos = 0;       // a counter for setupmode
//int direction = LOW;      // an indicator for down (LOW) or up (HIGH)


bool SERVOSMOVING = false;  // this is set if servos are still moving
bool buttonloop = false;

Button PROGBUTTON(14); //Pin A0
Button LOWBUTTON(15);  //Pin A1
Button HIGHBUTTON(16); //Pin A2
Button MODEBUTTON(17); //Pin A3



void CheckButtons() {
  
  if (PROGBUTTON.pressed()) {  // Go into program mode
      if (SetupMode == false) {
          SetupMode = true;
          thisservonumber = 0;
          SetProgramMode();
      }
      else {
          UpdateLimitData();
          thisservonumber = thisservonumber + 1;
          SetProgramMode();
          if (thisservonumber >= SERVOS){
            ReleaseProgramMode();
            SetupMode = false;
          }
          
      }
  }

  if (LOWBUTTON.pressed()) {
    Serial.println("The LOWBUTTON is pressed");
    if (SetupMode == true) {
      buttonloop = true;
      SetLow();
      while (buttonloop == true) {
        delay(150);
        if (LOWBUTTON.released()) {
          buttonloop = false;
          Serial.println("Button 1 released");
        }
        else {
          Serial.println("LOWBUTTON HELD");
          SetLow();
        }
      }
    }
  }

  if (HIGHBUTTON.pressed()) {
    Serial.println("The HIGHBUTTON is pressed");
    if (SetupMode == true) {
      buttonloop = true;
      SetHigh();
      while (buttonloop == true) {
        delay(150);
        if (HIGHBUTTON.released()) {
          buttonloop = false;
          Serial.println("HIGHBUTTON released");
        }
        else {
          Serial.println("HIGHBUTTON HELD");
          SetHigh();
        }
      }
    }
  }

  if (MODEBUTTON.pressed()) {
    if (SetupMode == true) {
      ChangeMode();
    }
  }

}


void setup() {

  //debug_init();	// initialize the debugger
  //breakpoint();   // stop execution here
  
  Serial.begin(115200);

  thisservonumber = 0;  // which one are we dealing with

  // Define buttons for setup
  LOWBUTTON.begin();
  PROGBUTTON.begin();
  HIGHBUTTON.begin();
  MODEBUTTON.begin();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  Serial.println("Read data from EEPROM: ");

  GetEEPROMData();
  BuildServoData();   // build the array used by Servo Driver

  //Start the DCC monitor
  DCCsetup();
 
  

}


void loop() {

  buttonloop = true; // a flag to control held buttons
  
    if (SetupMode == false){ // dont do this section if we are in setupmode

      // Check for a DCC command
      DCC.loop();

      if (ADDRESSRECEIVED){
        processAddress(thisservonumber);
      }

      processServos();
      
    }
    else {
    CheckFlash();   // Check if the LED needs flashing
    }

  if (!SERVOSMOVING) {    /// dont check the buttons whilst servos are moving

    CheckButtons();

  }
}

