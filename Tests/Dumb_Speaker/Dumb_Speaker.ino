void setup() {
  // Set pin D6 as output (connected to OC0A)
  DDRD = 1 << DDD6;
  // Set PWM value /255
  OCR0A = 127;
  // Set TCCR0A as Fast PWM
  TCCR0A = (1 << WGM01) | (1 << WGM00);
  TCCR0B = (0 << WGM02);
  // Set at bottom, clear on compare mach
  TCCR0A |= (1 << COM0A1) | (0 << COM0A0);
  // Set clock source
  TCCR0B |=  (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void loop() {
  // put your main code here, to run repeatedly
}
