struct RTC {
  uint8_t weekday : 3;
  uint8_t hour    : 5;
  uint8_t minute  : 7;
  uint8_t second  : 7;
  uint8_t hte     : 7; // Hundred Twenty-Eighth of a second
};

struct RotaryEncoder {
  uint8_t A_prev : 1;
  uint8_t B_prev : 1;
  uint8_t A_first : 1;
  uint8_t B_first : 1;

  uint8_t A_PC : 1;
  uint8_t B_PC : 1;
  uint8_t A_first_bkp : 1;

  int8_t value;
};

struct Buttons {
  uint8_t enter : 1;
  uint8_t back : 1;
  uint8_t bright : 1;
  uint8_t enter_prev : 1;
  uint8_t back_prev : 1;
  uint8_t bright_prev : 1;
};

union PortBuffer {
  uint8_t raw;
  struct {
    uint8_t P0: 1;
    uint8_t P1: 1;
    uint8_t P2: 1;
    uint8_t P3: 1;
    uint8_t P4: 1;
    uint8_t P5: 1;
    uint8_t P6: 1;
    uint8_t P7: 1;
  };
};

struct Alarm {
  uint8_t weekday : 3;
  uint8_t hour    : 5;
  uint8_t minute  : 7;
};
