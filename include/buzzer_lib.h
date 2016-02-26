#ifndef BUZZER_LIB_H
#define BUZZER_LIB_H

#define BASE_FREQ 500

#define ZERO_LATCH 0x00
#define ON_LATCH 0x01

#define STATE_MAX 100
#define STATE_MIN 0

#define DEFAULT_PIN 1

int setup_buzzer_lib(int pin);
int set_buzzer_state(int state);
int set_beep_mode(unsigned char state_a, unsigned char state_b, int half_period);
void turn_off_beeper();

#endif
