/** @file	hardwarePWM.c
 * 	@brief Handles the PWM output that controls the servo and ESC.
 */

#include <hal.h>
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static int map(int x, int in_min, int in_max, int out_min, int out_max);

/**
 * Configuration for the PWM driver.
 */
static PWMConfig pwmcfg = {
	1000000, // 1Mhz freq
    20000,   // 20 ms period
	NULL,
	{ // Move this to config
			{PWM_OUTPUT_ACTIVE_HIGH, NULL}, // Channel 1 and 2 active
			{PWM_OUTPUT_ACTIVE_HIGH, NULL},
			{PWM_OUTPUT_DISABLED, NULL},
			{PWM_OUTPUT_DISABLED, NULL}
	},
	0,
	0
};


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pins etc.
 */
void hardwarePWMSetup(void) {
	palSetPadMode(PWM_PIN_GROUPS[0], PWM_PIN_NUMBERS[0], PAL_MODE_ALTERNATE(2));
	palSetPadMode(PWM_PIN_GROUPS[1], PWM_PIN_NUMBERS[1], PAL_MODE_ALTERNATE(2));
	pwmStart(PWM_TIMER, &pwmcfg);
	pwmEnableChannel(&PWMD5, 0, CTRL_OUT_SPEED_PULSEWIDTHS[SPEED_STOP]);
	pwmEnableChannel(&PWMD5, 1, CTRL_OUT_WHEELS_CENTERED_PW);
}


/*
 * Setter for the values. Specify an output channel ID.
 */
void hardwarePWMSetValues(PWM_OUTPUT_ID pwm_id, int value) {
	if (pwm_id == PWM_OUTPUT_SERVO) {
		// Map angle linearly to pulsewidth. Different mappings on either side,
		// based on the pulsewidths we perceived as producing the max steering
		// angles.
		if (value < 90) {
			pwmEnableChannel(&PWMD5, 1, map(value, CTRL_OUT_WHEELS_MAXLEFT_ANGLE,
					CTRL_OUT_WHEELS_CENTERED_ANGLE, CTRL_OUT_WHEELS_MAXLEFT_PW, CTRL_OUT_WHEELS_CENTERED_PW));
		} else if (value > 90) {
			pwmEnableChannel(&PWMD5, 1, map(value, CTRL_OUT_WHEELS_CENTERED_ANGLE,
					CTRL_OUT_WHEELS_MAXRIGHT_ANGLE, CTRL_OUT_WHEELS_CENTERED_PW, CTRL_OUT_WHEELS_MAXRIGHT_PW));
		} else {
			// Center
			pwmEnableChannel(&PWMD5, 1, CTRL_OUT_WHEELS_CENTERED_PW);
		}
	} else if (pwm_id == PWM_OUTPUT_ESC) {
		// Set ESC pw to what is stored at SPEED_PULSEWIDTHS[value] provided that
		// "value" is in the valid range of speed steps
		if (value >= 0 && value <= CTRL_OUT_SPEED_STEPS - 1) {
			pwmEnableChannel(&PWMD5, 0, CTRL_OUT_SPEED_PULSEWIDTHS[value]);
		}
	}
}


/*
 * Setter for the values, pulse widths directly from RC transmitter.
 */
void hardwarePWMSetValuesRC(icucnt_t throttle, icucnt_t steering) {
	pwmEnableChannel(&PWMD5, 0, throttle);
	pwmEnableChannel(&PWMD5, 1, steering);
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------

/**
 * Map function, actually borrowed from the Arduino reference manual!
 * Adapted to not allow out of bound values.
 */
static int map(int x, int in_min, int in_max, int out_min, int out_max) {
	if (x < in_min) x = in_min;
	if (x > in_max) x = in_max;
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
