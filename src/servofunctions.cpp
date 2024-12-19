
#include "DCCServo.h"
#include "servofunctions.h"


// A few more variables to control the LED flash
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
int LEDSTATE = LOW;                     // Used to check if the LED is on or off
int LEDFLASHES = 0;
// constants won't change:
const long flashInterval = 10000;           // interval at which to blink (milliseconds)

 // General variables to control the logic
int pos = 0;          // variable to store the servo position
int lowpos = 0;       // variable to store to off position
int midpos = 0;       // variable to store the mod position
int highpos = 0;      // variable to store the on position
bool revflag = false; // flag to indicate if action is reversed
int eeAddress = 0;    // varable for EEPROM address

int currentpos = 0;       // a counter for setupmode
int direction = LOW;      // an indicator for down (LOW) or up (HIGH)
int thisservonumber = 0;  // which one are we dealing with
unsigned long previousMilis = 0;
//const unsigned int refreshInterval = 50;
bool SomethingsChanged = false; // a flag to ensure we only write back changed data

const unsigned int refreshInterval = 50;
unsigned long lastRefresh = 0;

// create the servo objects
Servo myServo[SERVOS];

// define the data arrays
ServoLimits servodata[10];
ServoPositionData servopos[10];
ServoData sData[10];




void CheckFlash() {
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= flashInterval) {
    // save the last time you blinked the LED
    Serial.print("Flashing the LED at servo number ");
    Serial.println(thisservonumber);
    previousMillis = currentMillis;

    for (int x = 1; x <= (thisservonumber + 1); x++) {
      Serial.println(x);
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(100);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(200);   
    }                  
  }
}


void WriteToServo(uint8_t servo, uint16_t value) {
  if (value >= 450 && value <= 2500) {
    myServo[servo].writeMicroseconds(pos);
  }
}



void GetEEPROMData() {
   // when setting up the max low/high value should be 450 / 2500
   // mid value 1025
   //  suggest default to be  800 / 1250

  Serial.println("Read data from EEPROM: ");

  eeAddress = 0; //Position at start of EEPROM.
  
  for (int x = 0; x < SERVOS; x++) { 

    EEPROM.get(eeAddress, servodata[x]);
 
    Serial.print("Servo Number  ");
    Serial.print(servodata[x].servonum);
    Serial.print("  ");
    Serial.println(servodata[x].lowvalue);
    Serial.print("  ");
    Serial.println(servodata[x].highvalue);
    Serial.print("  ");
    Serial.println(servodata[x].reverseflag);
    eeAddress += 12;
  }
  
}




void SetProgramMode() {
      
  Serial.println("Setting Program Mode");
  //midpos = 90; // variable to store the mod position
  midpos = 1025; // variable to store the mod position
  SomethingsChanged = false; // a flag to check for changes
  currentpos = midpos;
  Serial.print("Current Position - ");
  Serial.println(currentpos);
  WriteToServo(thisservonumber, currentpos);
 
}

void SetLow(){

  Serial.println("Setting Low Routine ");
  if (direction == HIGH) { // we are changing cirection so save the High
    highpos = currentpos;
  }
    currentpos = currentpos -1;
    direction = LOW;
    Serial.print("Current Position - ");
    Serial.println(currentpos);
    WriteToServo(thisservonumber, currentpos);
    SomethingsChanged = true; // changeshave been made
    lowpos = currentpos;
  
  
}

void SetHigh(){

  Serial.println("Set High Routine ");
   if (direction == LOW) { // we are changing cirection so save the Low
    lowpos = currentpos;
  }
  
    currentpos = currentpos + 1;
    direction = HIGH;
    Serial.print("Current Position - ");
    Serial.println(currentpos);
    WriteToServo(thisservonumber, currentpos);
    SomethingsChanged = true; // changes have been made
    highpos = currentpos;
  
}

void ChangeMode() {

    Serial.println("Setting the Reverse Flag ");
   // we need to set the reverse flag - toggle it.
   //

   SomethingsChanged = true; // changeshave been made
   if (revflag == false) {
     revflag = true;
   }
   else {
      revflag = false;
   }

}

void UpdateLimitData(){

   if (SomethingsChanged == true) { // Only update if the user has changed something
    
  
  // Update the data and write back to the EEprom
  // and rebuild the servoData array

    servodata[thisservonumber].lowvalue = lowpos;
    servodata[thisservonumber].highvalue = highpos;
    servodata[thisservonumber].reverseflag = revflag;

    Serial.println("Writing new values to EEPROM: ");
    Serial.print("For Servo Number : ");
    Serial.println(thisservonumber);
    Serial.println(servodata[thisservonumber].lowvalue);
    Serial.println(servodata[thisservonumber].highvalue);
    Serial.println(servodata[thisservonumber].reverseflag);

    eeAddress = 0; // the array starts at position zero
    eeAddress += (thisservonumber * 12);
    Serial.print("At address : ");
    Serial.println(eeAddress);
    EEPROM.put(eeAddress, servodata[thisservonumber]);

    SomethingsChanged = false;
   }
}

void ReleaseProgramMode(){

  Serial.println("Releasing Program Mode");

  SomethingsChanged = false; // a flag to check for changes
  currentpos = lowpos;
  Serial.print("Current Position - ");
  Serial.println(currentpos);
  WriteToServo(thisservonumber, currentpos);
    
}

void processAddress(uint8_t x){
      // Check the servo is not currently moving
      //if it is ignore the packet.
      if (sData[x].stepNumber == 0) { // not moving so we can process it
          
          //sData[x].status = SERVO_MOVING;
          switch (ADDRESSACTIVATE){
              case true: 
                  sData[x].status = SERVO_THROWING;
                  sData[x].fromPosition = sData[x].inactivePosition;
                  sData[x].toPosition = sData[x].activePosition;
              break;
              case false:
                  sData[x].status = SERVO_CLOSING;
                  sData[x].fromPosition = sData[x].activePosition;
                  sData[x].toPosition = sData[x].inactivePosition; 
                  sData[x].numSteps = sData[x].activePosition - sData[x].inactivePosition ;
              break;
          }
          sData[x].stepNumber = 1;
      } 

}

void processServos() {
  if (millis() - lastRefresh > refreshInterval) {
    lastRefresh = millis();
    for (uint8_t x = 0; x < SERVOS; x++) {
      if (sData[x].stepNumber != 0) {  // the servo needs to move
        updatePosition(x);
      }
    }
  }
}

void updatePosition(uint8_t x) {
  
  if (sData[x].numSteps == 0) {
    return; // No animation in progress
  }

  if (sData[x].stepNumber == 0 && sData[x].fromPosition == sData[x].toPosition) {
    // Go straight to end of sequence, output final position.
    //bitSet(digitalPinStates[pinByte], pinBit);
    sData[x].stepNumber = sData[x].numSteps-1;
  }

  if (sData[x].stepNumber < sData[x].numSteps) {
    //bitSet(digitalPinStates[pinByte], pinBit);
    // Animation in progress, reposition servo
    sData[x].stepNumber++;
    if ((sData[x].currentProfile & ~SERVO_NOPOWEROFF) == SERVO_BOUNCE) {
      // Retrieve step positions from array in flash
      uint8_t profileValue = bounceProfile[sData[x].stepNumber];
      sData[x].currentPosition = map(profileValue, 0, 100, sData[x].fromPosition, sData[x].toPosition);
    } else {
      // All other profiles - calculate step by linear interpolation between from and to positions.
      sData[x].currentPosition = map(sData[x].stepNumber, 0, sData[x].numSteps, sData[x].fromPosition, sData[x].toPosition);
    }
    // Send servo command
    WriteToServo(sData[x].pin, sData[x].currentPosition);
    //writePWM(pin, sData[x].currentPosition);
  } else if (sData[x].stepNumber < sData[x].numSteps + _catchupSteps) {
    // We've finished animation, wait a little to allow servo to catch up
    sData[x].stepNumber++;
  } else if (sData[x].stepNumber == sData[x].numSteps + _catchupSteps 
            && sData[x].currentPosition != 0) {
    //bitClear(digitalPinStates[pinByte], pinBit);
    sData[x].numSteps = 0;  // Done now.
  }
}

void BuildServoData() {

  for (uint8_t x = 0; x < 10; x++) {
    
      // Servo pin not configured, so configure now using defaults
      //s = sData[x] = (struct ServoData *) calloc(sizeof(struct ServoData), 1);
      //if (s == NULL) return false;  // Check for memory allocation failure
      sData[x].servonum = servodata[x].servonum;
      sData[x].pin = servodata[x].pin;
      sData[x].status = SERVO_CLOSED;
      sData[x].activePosition = servodata[x].highvalue;
      sData[x].inactivePosition = servodata[x].lowvalue;
      sData[x].currentPosition = 0;
      //sData[x].profile = SERVO_INSTANT | SERVO_NOPOWEROFF;  // Use instant profile (but not this time)
      sData[x].profile = servodata[x].mode;
    
      //}
      // Animated profile.  Initiate the appropriate action.
      sData[x].currentProfile = sData[x].profile;
      calcSteps(x, sData[x].currentProfile);
      sData[x].stepNumber = 0;
      sData[x].toPosition = 0;
      sData[x].fromPosition = sData[x].currentPosition;
      return;
    
    
  }
}

void calcSteps(uint8_t x, uint8_t currentProfile) {
    uint8_t profileValue = currentProfile & ~SERVO_NOPOWEROFF;  // Mask off 'don't-power-off' bit.
    sData[x].numSteps = profileValue==SERVO_FAST ? 10 :   // 0.5 seconds
                  profileValue==SERVO_MEDIUM ? 20 : // 1.0 seconds
                  profileValue==SERVO_SLOW ? 40 :   // 2.0 seconds
                  profileValue==SERVO_BOUNCE ? sizeof(bounceProfile)-1 : // ~ 1.5 seconds
                  sData[x].duration * 2 + 1; // Convert from deciseconds (100ms) to refresh cycles (50ms)
}


