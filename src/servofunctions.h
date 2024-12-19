/*
 *  © 2023, Peter Cole. All rights reserved.
 *  
 *  This file is part of EX-IOExpander.
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SERVO_FUNCTIONS_H
#define SERVO_FUNCTIONS_H



static const uint8_t _catchupSteps = 5;
extern const unsigned int refreshInterval;
extern unsigned long lastRefresh;
const uint8_t bounceProfile[30] = 
  {0,2,3,7,13,33,50,83,100,83,75,70,65,60,60,65,74,84,100,83,75,70,70,72,75,80,87,92,97,100};

  struct ServoLimits {
    int servonum;         //2
    int pin;              //2
    int address;          //2
    int lowvalue;         //2         CLOSED VALUE
    int highvalue;        //2         THROWN VALUE
    byte mode;            //1
    bool reverseflag;     //1   Total 12    
  }; //Array to store custom data read from EEPROM.

  struct ServoPositionData {
    int servonum;
    int status;
    int direction;
    int position;
  }; // Array for position data

  /*
  Servo data struct
  */
  struct ServoData {
    uint8_t servonum;
    uint8_t pin;
    uint16_t address;
    uint8_t status;
    uint16_t activePosition; 
    uint16_t inactivePosition;
    uint16_t currentPosition;
    uint16_t fromPosition;
    uint16_t toPosition; 
    uint8_t profile; 
    uint16_t stepNumber; // Index of current step (starting from 0)
    uint16_t numSteps;  // Number of steps in animation, or 0 if none in progress.
    uint8_t currentProfile; // profile being used for current animation.
    uint16_t duration; // time (tenths of a second) for animation to complete.
  }; // 23 bytes per element, i.e. per pin in use


  enum servoState {
    SERVO_CLOSED,
    SERVO_CLOSING,
    SERVO_MOVING,
    SERVO_THROWN,
    SERVO_THROWING
  };

  /////////////////////////////////////////////////////////////////////////////////////
  //  Define the servo profile type values
  //  These are reversed fromthe CS as default is 1 SLOW for turnout.
  #define SERVO_INSTANT 0x00        // Moves immediately between positions (if duration not specified)
  #define SERVO_USEDURATION 0x00    // Use specified duration
  #define SERVO_SLOW 0x01         // Takes around 500ms end-to-end
  #define SERVO_MEDIUM 0x02       // 1 second end-to-end
  #define SERVO_FAST 0x03         // 2 seconds end-to-end
  #define SERVO_BOUNCE 0x04       // For semaphores/turnouts with a bit of bounce!!
  #define SERVO_NOPOWEROFF 0x80   // Flag to power off after move complete

void processAddress();
void processServos();
void updatePosition(uint8_t pin);
void writePWM(uint8_t pin, uint16_t value);

#endif