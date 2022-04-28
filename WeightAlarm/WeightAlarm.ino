#include <LiquidCrystal_74HC595.h>
#include <SdFat.h>
#include <MCP4131.h>
#include <TMRpcm.h>

#include "FileHandling.h"
#include "datatypes.h"
#include "data.h"


#define dprint(a) Serial.print(#a " = "); Serial.println(a);
#define SET_LCD_BACKLIGHT(a) OCR2B = a

struct flag {
  uint8_t second_update : 1;
  uint8_t alarm : 1;
  uint8_t ring : 1;
} flag;

/* User Inputs */
struct RTC rtc = {0, 6, 59, 45};
struct RotaryEncoder re;
struct Buttons btn;

/* Peripherals */
#define SD_CONFIG SdSpiConfig(10, SD_SCK_MHZ(50))
SdFat32 sd;
MCP4131 digipot(8);
TMRpcm tmrpcm;
LiquidCrystal_74HC595 lcd(5, 6, 7, 2, 3, 4, 5, 6, 7);

/* Variables */
char filename[13];
char filefqn[22];


// TEMPORARY STUFF FOR PI3
const uint16_t human_weight_thresh = 410;
const uint16_t bed_weight_thresh = 700;
char lcd_buff[21];
struct Alarm alarm1 = { 0, 7, 0 };

void setup() {
  // Set CPU frequency to 16MHz
  OSCCAL = 0xFF;

  // Initialize LCD
  lcd.begin(20, 4);

  // Set PD2/PCINT18 and PD4/PCINT20 as input for Rotary Encoder
  DDRD &= ~((1 << PD4) | (1 << PD2));
  PCICR |= (1 << PCIE2);
  PCMSK2 = (1 << PCINT20) | (1 << PCINT18);

  // Initialize rotary encoder variables
  // If encoder is between 2 steps, assume a CW rotation
  re.A_prev = (PIND >> PIND2) & 1;
  re.B_prev = (PIND >> PIND4) & 1;
  re.A_first = 0;
  re.B_first = re.A_prev ^ re.B_prev;

  // Set PC2/PCINT10, PC3/PCINT11 and PC4/PCINT12 as input for buttons
  DDRC &= ~((1 << PC4) | (1 << PC3) | (1 << PC2));
  PCICR |= (1 << PCIE1);
  PCMSK1 = (1 << PCINT12) | (1 << PCINT11) | (1 << PCINT10);

  // Initialize the button variables
  btn.enter_prev = btn.enter = (~PINC >> PC4) & 1;
  btn.bright_prev = btn.bright = (~PINC >> PC3) & 1;
  btn.back_prev = btn.back = (~PINC >> PC2) & 1;

  // Setup Timer/Counter2 as RTC
  ASSR = (1 << AS2);
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << CS20);
  TIMSK2 = (1 << TOIE2);

  // Set OC2B/PD3 pin as PWM output for LCD backlight
  DDRD |= (1 << PD3);
  SET_LCD_BACKLIGHT(255);

  // Initialize TMRPCM
  tmrpcm.speakerPin = 9;
  
  // Initialize SD Card
  if (!sd.begin(SD_CONFIG)) {
    lcd.setCursor(0, 1);
    lcd.print("Could not read SD Card");
  }

  // Initialize the Digital Potentiometer
  digipot.writeWiper(127);
  digipot.writeWiper(127);
  
  sei();

  // TEMPORARY STUFF FOR PI3
  lcd.setCursor(1,1);
  lcd.print("Temps:");
  sprintf(lcd_buff, "Alarme: %s %02d:%02d", weekday_names[alarm1.weekday], alarm1.hour, alarm1.minute);
  lcd.setCursor(2,3);
  lcd.print(lcd_buff);
//  Serial.begin(2000000);
}

void print_time();

void loop() {
  if (rtc.hte && !flag.second_update) {
    flag.second_update = 1;
  } else if (!rtc.hte && flag.second_update) {
    flag.second_update = 0;
    // DEBUG start
    uint16_t weight = analogRead(A0);
    lcd.setCursor(0,0);
    lcd.print("    ");
    lcd.setCursor(0,0);
    lcd.print(weight);
    // DEBUG end
    print_time();
    if (rtc.weekday == alarm1.weekday && rtc.hour == alarm1.hour && rtc.minute == alarm1.minute && !flag.alarm) {
      flag.alarm = 1;
      lcd.setCursor(7,0);
      lcd.print("DEBOUT!");
    } else if (rtc.weekday == alarm1.weekday && rtc.hour == alarm1.hour && rtc.minute == (alarm1.minute+1) && flag.alarm) {
      flag.alarm = 0;
      lcd.setCursor(7,0);
      lcd.print("       ");
    }
    if (flag.alarm && (weight < human_weight_thresh || weight > bed_weight_thresh) && !flag.ring) {
      flag.ring = 1;
      //getrandomfile(filefqn, filename, "/Alarms", ".wav");
      tmrpcm.play("Alarms/rosaloud.wav");
    } else if (weight > human_weight_thresh && weight < bed_weight_thresh && flag.ring) {
      flag.ring = 0;
      tmrpcm.stopPlayback();
    }
  }
  
  // Adjust LCD brightness on button press
  if (btn.bright) {
    OCR2B += (OCR2B != 255) ? 255/5 : 1;
    btn.bright = 0;
  }
}

void print_time() {
  sprintf(lcd_buff, "%s %02d:%02d:%02d", weekday_names[rtc.weekday], rtc.hour, rtc.minute, rtc.second);
  lcd.setCursor(8, 1);
  lcd.print(lcd_buff);
}

ISR(TIMER2_OVF_vect) {
  rtc.hte++;
  rtc.second  = (rtc.second + !rtc.hte)%60;
  rtc.minute  = (rtc.minute + !(rtc.hte || rtc.second))%60;
  rtc.hour    = (rtc.hour + !(rtc.hte || rtc.second || rtc.minute))%24;
  rtc.weekday = (rtc.weekday + !(rtc.hte || rtc.second || rtc.minute || rtc.hour))%7;
}

ISR(PCINT2_vect) {
  /* Rotary encoder handling
  RE_X_prev: Rotary encoder sensor X previous state
  RE_X_first: Rotary encoder sensor X is first to change
  RE_X_PC: Rotary encoder sensor X pin change
  
  Carnaugh table:
  re.B_first  re.A_first  re.B_PC  re.A_PC | re.B_first  re.A_first  re.value
  0           0           0        1       | 0           1           0
  0           0           1        0       | 1           0           0
  0           1           0        1       | 0           0           0
  0           1           1        0       | 0           0           --
  1           0           0        1       | 0           0           ++
  1           0           1        0       | 0           0           0
  */
  re.A_PC = re.A_prev != ((PIND >> PIND2) & 1);
  re.B_PC = re.B_prev != ((PIND >> PIND4) & 1);

  // PCINT2 is sometimes triggered even though there is no noticeable pin change
  if (!(re.A_PC || re.B_PC))
    return;

  re.value += (re.A_PC && re.B_first) - (re.B_PC && re.A_first);
  
  re.A_first_bkp = re.A_first;
  re.A_first = !re.A_first && !re.B_first && !re.B_PC && re.A_PC;
  re.B_first = !re.A_first_bkp && !re.B_first && re.B_PC && !re.A_PC;

  // Update last values 
  re.A_prev ^= re.A_PC;
  re.B_prev ^= re.B_PC;
}

ISR(PCINT1_vect) {
  /* Button pin assignment:
  PC2/PCINT10: bright
  PC3/PCINT11: back
  PC4/PCINT12: enter
  */
  // Copy the content of PORTC to make sure it doesn't change mid-interrupt
  PortBuffer buff = {~PINC};

  if (buff.P2 != btn.bright_prev)
    btn.bright_prev = btn.bright = buff.P2;

  if (buff.P3 != btn.back_prev)
    btn.back_prev = btn.back = buff.P3;

  if (buff.P4 != btn.enter_prev)
    btn.enter_prev = btn.enter = buff.P4;
}
