/**
 * Functions to control and display the led 4 digit 7 segment display as well as the 4 led alarm
 */

#include "led_driver.h"

int digit_mask(int digit);
int led_segments(int value, int* segments);
int write_digit(int value, int digit);
int write_float(float value);

//Clock display variables
float current_value = 000.0;
int current_digit = 0;
int digits[4] = {0,0,0,0};

//alarm variables
int alarm_on = 0;
int led_pins[4] = {GPIO_PIN_14, GPIO_PIN_15, GPIO_PIN_12, GPIO_PIN_13};
int current_led = 0;

///ALARM FUNCTIONS

/**
 * Sets the current ON led to OFF and sets the next led to ON on a clockwise manner
 * Use to turn ON alarm as well
 */
void ALARM_switch(void){
	int next = (current_led + 1) % 4;
	alarm_on = 1;

	HAL_GPIO_WritePin(GPIOD, led_pins[next], GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, led_pins[current_led], GPIO_PIN_RESET);

	current_led = next;
}

/**
 * Disable the alarm (all LEDs off)
 */
void ALARM_off(void){
	if (alarm_on) {
		HAL_GPIO_WritePin(GPIOD, led_pins[current_led], GPIO_PIN_RESET);
		current_led = 0;
		alarm_on = 0;
	}
}

//CLOCK DISPLAY FUNCTIONS
/**
 * Displays the current temp value on the 4digit display
 * @return 0 if no errors
 */
int LED_display(){
	write_digit(digits[0], 0);
	write_digit(digits[1], 1);
	write_digit(digits[2], 2);
	write_digit(digits[3], 3);

	return 0;
}

/**
 * Set the private temp value to be displayed on next refresh
 * @param new_value new value to set temp to
 */
void LED_set_value(float new_value){
	int base_int = (int)(current_value * 10);
	current_value = new_value;

	digits[0] = base_int % 10;
	digits[1] = (base_int / 10) % 10;
	digits[2] = (base_int / 100) % 10;
	digits[3] = (base_int / 1000) % 10;
}

/**
 * Get current private temp value
 * @return  current temp
 */
float LED_get_value(void){
	return current_value;
}

/**
 * Writes a float value to the display
 * @param  value float value to display
 * @return       error code (0 if no errors)
 */
int write_float(float value){
	int base_int = (int)(value * 10);
	int decimal = base_int % 10;
	int digit_1 = (base_int / 10) % 10;
	int digit_2 = (base_int / 100) % 10;
	int digit_3 = (base_int / 1000) % 10;

	write_digit(decimal, 0);
	write_digit(digit_1, 1);
	write_digit(digit_2, 2);
	write_digit(digit_3, 3);

	return 0;
}

/**
 * Writes one digit to one of the 4 7 segment digits
 * @param  value values to display
 * @param  digit which digit to display it on (0 is right most)
 * @return       error code (0 if no errors)
 */
int write_digit(int value, int digit){
	int segments[7];
	int valid = 0;
	int cathode;

	if(digit == 0) cathode = cathode4;
	else if(digit == 1) cathode = cathode3;
	else if(digit == 2) cathode = cathode2;
	else if(digit == 3) cathode = cathode1;
	else return -1;

	HAL_GPIO_WritePin(LED_PORT, cathode, GPIO_PIN_SET);

	if(digit == 1) HAL_GPIO_WritePin(LED_PORT, pinDP, GPIO_PIN_SET);

	valid = led_segments(value, segments);

	if (segments[0]) HAL_GPIO_WritePin(LED_PORT, pinA, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinA, GPIO_PIN_RESET);

	if (segments[1]) HAL_GPIO_WritePin(LED_PORT, pinB, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinB, GPIO_PIN_RESET);

	if (segments[2]) HAL_GPIO_WritePin(LED_PORT, pinC, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinC, GPIO_PIN_RESET);

	if (segments[3]) HAL_GPIO_WritePin(LED_PORT, pinD, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinD, GPIO_PIN_RESET);

	if (segments[4]) HAL_GPIO_WritePin(LED_PORT, pinE, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinE, GPIO_PIN_RESET);

	if (segments[5]) HAL_GPIO_WritePin(LED_PORT, pinF, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinF, GPIO_PIN_RESET);

	if (segments[6]) HAL_GPIO_WritePin(LED_PORT, pinG, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LED_PORT, pinG, GPIO_PIN_RESET);

	HAL_Delay(1);
	HAL_GPIO_WritePin(LED_PORT, cathode, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_PORT, pinDP, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_PORT, pinA|pinB|pinC|pinD|pinE|pinF|pinG, GPIO_PIN_RESET);

	return valid;
}

/**
 * Returns an array of the segments to switch on ([0] is A, [6] is G)
 * @param  value    int value to convert
 * @param  segments array to store result in
 * @return          error code (0 if no errors)
 */
int led_segments(int value, int* segments){
	int mask = digit_mask(value);
	segments[0] = mask & segA;
	segments[1] = mask & segB;
	segments[2] = mask & segC;
	segments[3] = mask & segD;
	segments[4] = mask & segE;
	segments[5] = mask & segF;
	segments[6] = mask & segG;

	return mask;
}

int digit_mask(int value) {
	if (value == 0) return dig0;
	if (value == 1) return dig1;
	if (value == 2) return dig2;
	if (value == 3) return dig3;
	if (value == 4) return dig4;
	if (value == 5) return dig5;
	if (value == 6) return dig6;
	if (value == 7) return dig7;
	if (value == 8) return dig8;
	if (value == 9) return dig9;

	return -1;
}
