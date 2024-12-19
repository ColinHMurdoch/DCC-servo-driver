////////////////////////////////////////////////////////////////////////////////
//
// DCC Turntable Control Test Routines (Accessory Address 200)

#include "DCCServo.h"

#include <DCC_Decoder.h>


// Defines and structures
//
#define kDCC_INTERRUPT         2
bool ADDRESSRECEIVED = false;
bool ADDRESSACTIVATE = false;


typedef struct
{
    uint16_t               address;                // Address to respond to
} 
DCCAccessoryAddress;

DCCAccessoryAddress ServoAddress[SERVOS];


void ConfigureAddresses()
 {
    for (byte x = 0; x < SERVOS; x++){
        ServoAddress[x].address = STARTADDRESS + x;
    }
    
 }

// Basic accessory packet handler 
//
void BasicAccDecoderPacket_Handler(int32_t address, boolean activate, byte data)
{
        // Convert NMRA packet address format to human address
    address -= 1;
    address *= 4;
    address += 1;
    address += (data & 0x06) >> 1;
    
    boolean enable = (data & 0x01) ? 1 : 0;
    
    Serial.print(F(" Accessory Address Received"));
    Serial.println(address);
    Serial.print(F("Activate Status: "));
    Serial.println(enable, DEC);

    // Check if we have to action this address and if so update the sData struct

    for (int x = 0; x < SERVOS; x++){
        if(address = ServoAddress[x].address){
            ADDRESSRECEIVED = true;
            ADDRESSACTIVATE = enable;
            thisservonumber = x;
        }
        else {
            ADDRESSRECEIVED = false;
            ADDRESSACTIVATE = false;
            thisservonumber = 0;
        }
        
    }

}

void DCCsetup() 
{ 
   
   DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
   ConfigureAddresses();
   DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
}

