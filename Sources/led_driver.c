#include "led_driver.h"

int digit_mask(int digit);

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
	
	printf("%i", segments[0]);
	
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

	HAL_Delay(5);
	HAL_GPIO_WritePin(LED_PORT, cathode, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_PORT, pinDP, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_PORT, pinA|pinB|pinC|pinD|pinE|pinF|pinG, GPIO_PIN_RESET);

	return valid;
}

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
