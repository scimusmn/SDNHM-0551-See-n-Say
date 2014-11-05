/**
 * \file FilePlayer.ino
 *
 * \brief Example sketch of using the MP3Shield Arduino driver
 * \remarks comments are implemented with Doxygen Markdown format
 *
 * \author Bill Porter
 * \author Michael P. Flaga
 *
 * This sketch listens for commands from a serial terminal (such as the Serial
 * Monitor in the Arduino IDE). Listening for either a single character menu
 * commands or an numeric strings of an index. Pointing to a music file, found
 * in the root of the SdCard, to be played. A list of index's and corresponding
 * files in the root can be listed out using the 'l' (little L) command.
 *
 * This sketch allows the various file formats to be played: mp3, aac, wma, wav,
 * fla & mid.
 *
 * This sketch behaves nearly identical to MP3Shield_Library_Demo.ino, but has
 * extra complicated loop() as to recieve string of characters to create the
 * file index. As the Serial Monitor is typically default with no CR or LF, this
 * sketch uses intercharacter time out as to determine when a full string has
 * has been entered to be processed.
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>
#include <Wire.h>


/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;
int16_t last_ms_char; // milliseconds of last recieved character from Serial port.
int8_t buffer_pos; // next position to recieve character from Serial port.

//------------------------------------------------------------------------------
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 *
 * \note returned Error codes are typically passed up from MP3player.
 * Whicn in turns creates and initializes the SdCard objects.
 *
 * \see
 * \ref Error_Codes
 */
  char buffer[6]; // 0-35K+null

void setup() {

  uint8_t result; //result code from some function as to be tested at later time.

  Serial.begin(115200);

  Serial.print(F("Free RAM = ")); // available in Version 1.0 F() bases the string to into Flash, to use less SRAM.
  Serial.print(FreeRam(), DEC);  // FreeRam() is provided by SdFatUtil.h
  Serial.println(F(" Should be a base line of 1029, on ATmega328 when using INTx"));


  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();
  //check result, see readme for error codes.
  if(result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
    if( result == 6 ) {
      Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
      Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
    }
  }

  last_ms_char = millis(); // stroke the inter character timeout.
  buffer_pos = 0; // start the command string at zero length.
  
  Wire.begin(16);                // join i2c bus with address #16
  Wire.onReceive(receiveData); // register event
  Wire.onRequest(requested);
  
  MP3player.setVolume(20, 20);
  /*result = MP3player.playMP3("01Rsnake.mp3");
  
  if(result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to play track"));
    } else {

      Serial.println(F("Playing:"));
  
  char title[30];
  MP3player.trackTitle((char*)&title);

      //print out the arrays of track information
  Serial.write((byte*)&title, 30);
  Serial.println();
    }*/
}

void requested(){
  Wire.write(MP3player.getState()==playback);
}

char sounds[10][13] = {"06hrnowl.mp3", "05gryfox.mp3","09possum.mp3", \
                        "10skunky.mp3", "07raccon.mp3", "01Rsnake.mp3", \
                        "08weasel.mp3","04coyote.mp3","03brnowl.mp3", \
                        "02bobcat.mp3"\
                      };

int forcedDwell =0;

void receiveData(int howMany)
{
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  if(MP3player.getState()==playback) MP3player.stopTrack();
  if(x>=0&&x<=9){
    MP3player.playMP3(sounds[x]);
  }
  else MP3player.stopTrack();
}

int vol = 2;

void loop() {
  int volRead = map(analogRead(0),0,1023,60,2);
  
  if(abs(volRead-vol)>2){
    vol=volRead;
    MP3player.setVolume(vol, vol);
  }
}

