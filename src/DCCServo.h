#ifndef DCCSERVO_H
  #define DCCSERVO_H

  #include <Arduino.h>
  #include <Servo.h>
  #include <EEPROM.h>
  #include <DCC_Decoder.h>
  #include <Button.h>
  #include "servofunctions.h"


  #define SERVOS 10 // Set this value to the number of servos in use.
  #define BASE_PIN 3 // the first servo pin
  extern int STARTADDRESS; // Set this value to the start address
  extern bool ADDRESSRECEIVED; // indicates a valid address interpreted
  extern bool ADDRESSACTIVATE; // activate/deactivate
  extern int thisservonumber;  // which one are we dealing with




  void ConfigureAddresses();
  void BasicAccDecoderPacket_Handler(int32_t address, boolean activate, byte data);
  void DCCsetup();

  void CheckFlash();
  void WriteToServo(uint8_t servo, uint16_t value);
  void GetEEPROMData();
  void SetProgramMode();
  void SetLow();
  void SetHigh();
  void ChangeMode();
  void UpdateLimitData();
  void ReleaseProgramMode();
  void processAddress(uint8_t x);
  void processServos();
  void updatePosition(uint8_t x);
  void BuildServoData();
  void calcSteps(uint8_t x, uint8_t currentProfile);


#endif
