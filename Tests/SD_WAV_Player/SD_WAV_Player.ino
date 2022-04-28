#include <SD.h>                      // need to include the SD library
//#define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
#define SD_ChipSelectPin 10  //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h>           //  also need to include this library...
#include <SPI.h>

TMRpcm tmrpcm;   // create an object for use in this sketch


void setup(){
  OSCCAL = 0xFF;
//  OCR0A = 31;
//  OCR0B = 15;
//  TCCR0A = (1 << COM0B1) | (0 << COM0B0) | (1 << WGM01) | (1 << WGM00);
//  TCCR0B = (1 << WGM02) | (0 << CS02) | (0 << CS01) | (1 << CS00);
//  DDRD |= (1 << PD5);

  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

  Serial.begin(115200);
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }
//  Serial.print("Enter a file name...");
//  Serial.setTimeout(30000);
//  String filename = Serial.readStringUntil('\n');
//  Serial.print("\nPlaying music file '");
//  Serial.print(filename.c_str());
//  Serial.println("'");
//  tmrpcm.play(filename.c_str());
  Serial.println("Playing music...");
  tmrpcm.play("Alarms/holdline.wav"); //the sound file "music" will play each time the arduino powers up, or is reset
}



void loop(){  

  if(Serial.available()){    
    if(Serial.read() == 'p'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("music");
    }
  }

}
