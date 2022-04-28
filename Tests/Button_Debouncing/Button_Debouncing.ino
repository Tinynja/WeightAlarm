unsigned long start_time=0;

uint8_t counter=0;

void setup() {
  OSCCAL = 0xFF;
  Serial.begin(115200);
  Serial.println("Initialized!");
  
  DDRD = (0 << PD7);
  //PORTD = (1 << PD7);
  PCICR = (1 << PCIE2);
  PCMSK2 = (1 << PCINT23);
  sei();
}

void loop() {}

ISR(PCINT2_vect) {
  bool pin = (PIND >> PD7) & 1;
//  long new_time = micros();
//  Serial.print(pin);
//  Serial.print(", ");
//  Serial.println(new_time-start_time);
//  start_time = new_time;
  
  if (!pin) {
    counter++;
    Serial.println(counter);
  }

  // Verify the HIGH/LOW threshold with a pot
//  Serial.print(pin);
//  Serial.print(":");
//  Serial.println(analogRead(A0));
}
