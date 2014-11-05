#ifndef ADDR_MEM
#define ADDR_MEM

#include "Arduino.h"

const int numAddr=10;
const int addrSz=7;

class addressMemory{
private:
  uint8_t matchID[numAddr][addrSz];
public:
  addressMemory(){
    memset(matchID,0,sizeof(matchID));
  }
  
  void loadAddresses(){
    for(int j=0; j<numAddr; j++){
      for(int i=0; i<addrSz; i++){
        matchID[j][i] = EEPROM.read(i+addrSz*j);
      }
    }
  }
  
  int checkAddress(uint8_t * uid){
    int matchedAddr = -1;
    for(int j=0; j<numAddr; j++){
      bool matched = true;
      for(int i=0; i<addrSz; i++){
        if(uid[i]!=matchID[j][i]) matched=false;
      }
      if(matched) matchedAddr = j;
    }
    return matchedAddr;
  }
  
  int storeAddress(int addrNum, uint8_t * uid){
    for(int i=0; i<addrSz; i++){
       matchID[addrNum][i] = uid[i];
       EEPROM.write(i+addrSz*addrNum,uid[i]);
    }
  }
  
};

#endif
