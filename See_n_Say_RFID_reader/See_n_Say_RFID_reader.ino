#include <Wire.h>
#include <EEPROM.h>
#include "Adafruit_NFCShield_I2C.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

#include "addressMemory.h"
addressMemory animals;

int programPin = 12;
int selectPot = 0;

//Audio Arduino wire address:
#define AudArdAddr 16

void setup(void) {
  Serial.begin(115200);
  
  animals.loadAddresses();
  
  pinMode(programPin,INPUT_PULLUP);
  pinMode(13,OUTPUT);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // configure board to read RFID tags
  nfc.SAMConfig();
}

uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

int prevCard=-1;
long dwellTime=0;
bool played = false;

void loop(void) {
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  bool correctCard=false;
  if (success) {
    if(!digitalRead(programPin)){
      int anim = map(analogRead(selectPot),0,1023,0,9);
      animals.storeAddress(anim,uid);
      Serial.print("Stored address for ");
      Serial.println(anim,DEC);
    }
    
    int which = animals.checkAddress(uid);
    
    if(prevCard!=which){
      prevCard=which;
      dwellTime=millis();
      played=false;
    }
    
    if(which>=0&&dwellTime+500<millis()&&!played){
      Wire.beginTransmission(AudArdAddr);
      Wire.write(which);
      Wire.endTransmission();
      Serial.println(which);
      digitalWrite(13,HIGH);
      played=true;
    }
    else digitalWrite(13,LOW);
  }
  
  
  delay(100);
  //int anim = map(analogRead(selectPot),0,1000,0,9);
  //Serial.println(anim);
}
