/**
 * @file buzzer_lib.c
 * @author Frederic Simard (fred.simard@atlantsembedded.com)
 * @date Jan 2016
 * @brief Library to operate a piezo-buzzer originally made for the braintone app
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <ctype.h>
#include <math.h>

#include <wiringPi.h>
#include <softTone.h>
#include <buzzer_lib.h>

/*Pins used (wiringPi numbers)*/
int BUZZER_PIN = 1;

/*defines the frequency scale*/
/*settings are for a linear range between 500 and 1000Hz*/
static float baseline_freq = 500;

static char beep_mode_latch = 0x00;
static int beep_mode_half_period = 0;
static unsigned char beep_mode_state_a = 0x00;
static unsigned char beep_mode_state_b = 0x00;

void *beep_mode(void *param);

/**
 * int setup_buzzer_lib(void)
 * @brief sets up the buzzer
 * @return 0 for success, -1 for error
 */
int setup_buzzer_lib(int pin)
{

	if (pin < 0) {
		BUZZER_PIN = DEFAULT_PIN;
	} else {
		BUZZER_PIN = pin;
	}
	/*setup wiringPi */
	wiringPiSetup();

	/*create a buzzer and set it off */
	softToneCreate(BUZZER_PIN);
	turn_off_beeper();

	/*Set scheduling priority */
	piHiPri(99);

	return EXIT_SUCCESS;
}

/**
 * int set_buzzer_state(int state)
 * @brief Sets the buzzer to a level between 0 and 100
 * @param state
 * @return 0 for success
 */
int set_buzzer_state(int state)
{

	if (state > STATE_MAX) {
		state = STATE_MAX;
		softToneWrite(BUZZER_PIN, state * 5 + baseline_freq);
	} else if (state <= STATE_MIN) {
		softToneWrite(BUZZER_PIN, STATE_MIN);
	} else {
		softToneWrite(BUZZER_PIN, state * 5 + baseline_freq);
	}
	return EXIT_SUCCESS;
}

/**
 * int set_beep_mode(unsigned char state_a, unsigned char state_b, int half_period)
 * @brief Sets the beeper on an intermittent mode
 */
int set_beep_mode(unsigned char state_a, unsigned char state_b, int half_period)
{

	pthread_t thread;
	pthread_attr_t attr;

	beep_mode_half_period = half_period;
	beep_mode_state_a = state_a;
	beep_mode_state_b = state_b;

	if (!beep_mode_latch) {
		beep_mode_latch = ON_LATCH;

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, beep_mode, (void *)NULL);
		pthread_attr_destroy(&attr);
	}

	return EXIT_SUCCESS;
}

/**
 * void* beep_mode(void*)
 * @brief run the buzzer through the alternating states (thread)
 * @return NULL if beep_mode_latch is 0x00
 */
void *beep_mode(void *param __attribute__ ((unused)))
{

	while (beep_mode_latch) {
		set_buzzer_state(beep_mode_state_a);
		delay(beep_mode_half_period);
		set_buzzer_state(beep_mode_state_b);
		delay(beep_mode_half_period);
	}

	return NULL;
}

/**
 * turn_off_beep()
 * @brief Turn off beeper
 */
void turn_off_beeper()
{
	beep_mode_latch = ZERO_LATCH;
	softToneWrite(BUZZER_PIN, STATE_MIN);
}
