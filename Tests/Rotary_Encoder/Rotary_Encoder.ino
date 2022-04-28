struct RotaryEncoder {
  uint8_t A_prev : 1;
  uint8_t B_prev : 1;
  uint8_t A_first : 1;
  uint8_t B_first : 1;

  uint8_t A_PC : 1;
  uint8_t B_PC : 1;
  uint8_t A_first_bak : 1;

  int8_t value;
};

struct RotaryEncoder RE;
uint16_t RE_count=0;

void setup() {
  Serial.begin(115200);
  Serial.println(RE_count);

  // Set pins 2 and 3 as output
  DDRD = (0 << DDD3) | (0 << DDD2);
  PORTD = (0 << PORTD3) | (0 << PORTD2);

  // Initialize rotary encoder variables
  // If encoder is between 2 steps, assume a CW rotation
  RE.A_prev = (PIND >> PIND2) & 1;
  RE.B_prev = (PIND >> PIND3) & 1;
  RE.A_first = 0;
  RE.B_first = RE.A_prev ^ RE.B_prev;

  // Enable pin change interrupts
  PCICR = (1 << PCIE2);
  PCMSK2 = (1 << PCINT19) | (1 << PCINT18);
  sei();
}

void loop() {
  asm("nop");
}

ISR(PCINT2_vect) {
  /* Rotary encoder handling
  RE_X_prev: Rotary encoder sensor X previous state
  RE_X_first: Rotary encoder sensor X is first to change
  RE_X_PC: Rotary encoder sensor X pin change
  
  Carnaugh table:
  RE.B_first  RE.A_first  RE.B_PC  RE.A_PC | RE.B_first  RE.A_first  RE_Count
  0           0           0        1       | 0           1           0
  0           0           1        0       | 1           0           0
  0           1           0        1       | 0           0           0
  0           1           1        0       | 0           0           --
  1           0           0        1       | 0           0           ++
  1           0           1        0       | 0           0           0
  */
  RE.A_PC = RE.A_prev != ((PIND >> PIND2) & 1);
  RE.B_PC = RE.B_prev != ((PIND >> PIND3) & 1);

  // PCINT2 is sometimes triggered even though there is no noticeable pin change
  if (!(RE.A_PC || RE.B_PC))
    return;

  RE_count += (RE.A_PC && RE.B_first) - (RE.B_PC && RE.A_first);

  if ((RE.A_PC && RE.B_first) || (RE.B_PC && RE.A_first))
    Serial.println(RE_count);
  
  RE.A_first_bak = RE.A_first;
  RE.A_first = !RE.A_first && !RE.B_first && !RE.B_PC && RE.A_PC;
  RE.B_first = !RE.A_first_bak && !RE.B_first && RE.B_PC && !RE.A_PC;

  // Update last values 
  RE.A_prev ^= RE.A_PC;
  RE.B_prev ^= RE.B_PC;
}
