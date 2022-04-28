#include <MCP4131.h>

MCP4131 digipot(8);

void setup() {
  OSCCAL = 0xFF;
  Serial.begin(9600);

  Serial.print("Before: ");
  Serial.println(digipot.readWiper());

  digipot.writeWiper(127);
  Serial.print("After: ");
  Serial.println(digipot.readWiper());
}

void loop() {
//  digipot.incrementWiper();
//  delay(500);
}
