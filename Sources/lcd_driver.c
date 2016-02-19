/**
 * Functions to control the LCD display and have it display the current CPU temperature
 */

#include "lcd_driver.h"

//Private constants
//array od DB pins
int DB_PINS[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
//Bit value of the DB pins (used for comparisons)
int DB_MASK[8] = {
    0x0001,
    0x0002,
    0x0004,
    0x0008,
    0x0010,
    0x0020,
    0x0040,
    0x0080
};

//Private temp variable thats is displayed on update / display refesh
float current_temp = 0.0f;

//Private variables used to control and keep track of the state of the LCD
int dec, ones, tens;

int e_value = 0;
int waiting = 0;
int wait_start = 0;
int write_counter = 0;
int lcd_write_sequence[16] = {LCD_C, LCD_P, LCD_U, LCD_space, LCD_T, LCD_e, LCD_m, LCD_p, LCD_colon, LCD_space, LCD_num_top | 0, LCD_num_top | 0, LCD_dot, LCD_num_top | 0, LCD_deg, LCD_C};
const int sequence_size = 16;

/**
 * Sets the private temperature value that will be displayed by the LCD
 * @param temp temperature to display on next update
 */
void LCD_set_temp(float temp){
    int base_int = (int)(temp * 10);

    current_temp = temp;
    dec = base_int % 10;
    base_int /= 10;
    ones = base_int % 10;
    base_int /= 10;
    tens = base_int % 10;

    lcd_write_sequence[10] = LCD_num_top | tens;
    lcd_write_sequence[11] = LCD_num_top | ones;
    lcd_write_sequence[13] = LCD_num_top | dec;
}

/**
 * Set the value of the RS pin
 * @param bit 1 for high 0 for low
 */
void RS_set(int bit){
    HAL_GPIO_WritePin(LCD_CONTROLPORT, RS_PIN, bit ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * Set the value of the RW pin
 * @param bit 1 for high 0 for low
 */
void RW_set(int bit){
    HAL_GPIO_WritePin(LCD_CONTROLPORT, RW_PIN, bit ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * Set the value of the E pin
 * @param bit 1 for high 0 for low
 */
void E_set(int bit){
    HAL_GPIO_WritePin(LCD_CONTROLPORT, E_PIN, bit ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * Pulse the E pin with delays, do not use inside main loop as it uses hal delay and will prevent other systems from running
 * Used for initialization only
 */
void E_pulse(void){
	HAL_Delay(2);
    HAL_GPIO_WritePin(LCD_CONTROLPORT, E_PIN, GPIO_PIN_SET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(LCD_CONTROLPORT, E_PIN, GPIO_PIN_RESET);
	HAL_Delay(2);
}

/**
 * Execute a function or send a character to LCD mem, for initialization only
 * @param func_mask function or character code
 */
void DB_exec(int func_mask){
    int i = 0;
    for (i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(LCD_PORT, DB_PINS[i], (DB_MASK[i] & func_mask) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    E_pulse();
}

/**
 * Set the value of the DB pins to a function or character
 * @param func_mask int value of the 8 bit value to set in the pins (DB7 is LSB)
 */
void DB_set_pins(int func_mask){
    int i = 0;
    for (i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(LCD_PORT, DB_PINS[i], (DB_MASK[i] & func_mask) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

/**
 * Switch between function and data mode
 * @param mode LCD_INST_MODE for function mode, LCD_DATA_MODE for data mode
 */
void LCD_mode_set(int mode) {
	if (mode == LCD_INST_MODE) {
        HAL_GPIO_WritePin(LCD_CONTROLPORT, RS_PIN, GPIO_PIN_RESET);
    } else if (mode == LCD_DATA_MODE) {
        HAL_GPIO_WritePin(LCD_CONTROLPORT, RS_PIN, GPIO_PIN_SET);
    }
}

/**
 * Writes a character at the current cursor location (Delay function, do not use in main loop)
 * @param func_mask int representation of the 8bit value of the character
 */
void LCD_write_char(int func_mask){
    LCD_mode_set(LCD_DATA_MODE);
    DB_exec(func_mask);
		LCD_mode_set(LCD_INST_MODE);
}

/**
 * Display a temp value on the lce (Delay function, do not use in main loop)
 * @param temp temperature value
 */
void LCD_disp_temp(float temp){
    LCD_set_temp(temp);

    DB_exec(0x01);
    LCD_mode_set(LCD_DATA_MODE);
    DB_exec(LCD_C);
    DB_exec(LCD_P);
    DB_exec(LCD_U);
    DB_exec(LCD_space);
    DB_exec(LCD_T);
    DB_exec(LCD_e);
    DB_exec(LCD_m);
    DB_exec(LCD_p);
    DB_exec(LCD_colon);
    DB_exec(LCD_space);
    DB_exec(LCD_num_top | tens);
    DB_exec(LCD_num_top | ones);
    DB_exec(LCD_dot);
    DB_exec(LCD_num_top | dec);
    DB_exec(LCD_deg);
    DB_exec(LCD_C);
    LCD_mode_set(LCD_INST_MODE);
    DB_exec(0x04);
}

/**
 * Update function for the LCD in the main loop.
 * this function updates the lcd with respect to the systick timer.
 * it uses private flags to switch between waiting states and writing states.
 * This mode of operation does not block the other systems from running as no delays are used
 */
void LCD_update(void){
    //Check if waiting
    if (waiting) {
        //If waited long enough leave wait
        if (HAL_GetTick() - wait_start > LCD_E_DELAY) {
            //If in first half of E cycle toggle E and wait again
            if (e_value == 1) {
                E_set(0);
                e_value = 0;
                waiting = 1;
                wait_start = HAL_GetTick();
                return;
            } else {
                waiting = 0;
            }
        }
        //If not return and service other sytems
        else return;
    }

    //If reached end of display string, send cursor home and restart refreshing the display
    if (write_counter == sequence_size) {
        write_counter = 0;
        LCD_mode_set(LCD_INST_MODE);
        DB_set_pins(0x02);
        E_set(1);
        e_value = 1;
        waiting = 1;
        wait_start = HAL_GetTick();
        return;
    }

    //Write the current character to the display
    LCD_mode_set(LCD_DATA_MODE);
    DB_set_pins(lcd_write_sequence[write_counter]);
    write_counter++;
    E_set(1);
    e_value = 1;
    waiting = 1;
    wait_start = HAL_GetTick();
    return;
}

/**
 * Initialize display
 * Set values for cursor and blinker
 * Turn display on
 * clear display
 * send cursor home
 */
void LCD_init(void){
    RS_set(0);
    RW_set(0);

    DB_exec(0x3C);
    DB_exec(0x0C);
    DB_exec(0x04);
    DB_exec(0x01);
}
