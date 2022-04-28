#include <SdFat.h>
#include <MCP4131.h>
#include <TMRpcm.h>

#include "utils.h"
#include "FileHandling.h"

#define SD_CONFIG SdSpiConfig(10, SD_SCK_MHZ(50))

SdFat32 sd;
MCP4131 digipot(8);
TMRpcm tmrpcm;

char filename[13];
char filefqn[22];

void setup() {
  OSCCAL = 0xFF;
  Serial.begin(115200);
  
  tmrpcm.speakerPin = 9;
  if (!sd.begin(SD_CONFIG))
    Serial.println("Could not read SD Card");

  srand(TCNT0);
  getnthfileext(filename, "/Alarms", rand()%countfilesext("/Alarms", ".wav"), ".wav");
  sprintf(filefqn, "/Alarms/%s", filename);
  dprint(filefqn);
  tmrpcm.play(filefqn);
}

void loop() {
  dprint(analogRead(A5)/8);
  digipot.writeWiper(analogRead(A5)/8);
  delay(1000);
}
