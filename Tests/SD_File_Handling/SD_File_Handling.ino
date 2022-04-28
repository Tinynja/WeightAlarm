#include <SdFat.h>
//#include <sdios.h>
//#include <TMRpcm.h>

#include "utils.h"
#include "FileHandling.h"

const uint8_t SD_CS_PIN = 10;

#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

SdFat32 sd;
//File32 dir;
//File32 file;

//TMRpcm tmrpcm;

char filename[13];
char filefqn[22];

// Serial print stream
//ArduinoOutStream cout(Serial);



void setup() {
  OSCCAL = 0xFF;
  //tmrpcm.speakerPin = 9;
  Serial.begin(115200);
  
  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(SD_CONFIG)) {
    Serial.println("Could not read SD Card");
    //sd.initErrorHalt(&Serial);
  }

  //sd.ls(&Serial, '/');

  //dprint(sd.exists("/Alarms"));
  //dir.open("/Alarms");
  //dprint(dir.isDirectory());

  //dprint(countfilesext("/Alarms", ".wav"));
  srand(TCNT0);
  getnthfileext(filename, "/Alarms", rand()%countfilesext("/Alarms", ".wav"), ".wav");
  //dprint(filename);
  sprintf(filefqn, "/Alarms/%s", filename);
  dprint(filefqn);
  //tmrpcm.play(filefqn);
}

void loop() {}
