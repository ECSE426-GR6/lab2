#include "stm32f4xx_hal.h"

#define LCD_WAIT_DELAY 1
#define LCD_E_DELAY 5

#define LCD_PORT GPIOD
#define LCD_CONTROLPORT GPIOC

#define LCD_INST_MODE 0
#define LCD_DATA_MODE 1

#define RS_PIN GPIO_PIN_0
#define RW_PIN GPIO_PIN_1
#define E_PIN GPIO_PIN_2

//CPU Temp: 12.3dC

#define LCD_C 0x43
#define LCD_P 0x50
#define LCD_U 0x55
#define LCD_T 0x54
#define LCD_e 0x65
#define LCD_m 0x6D
#define LCD_p 0x70

#define LCD_colon 0x3A
#define LCD_space 0xFE
#define LCD_dot 0x2E
#define LCD_deg 0xDF

#define LCD_num_top 0x30

void LCD_init(void);
void LCD_disp_temp(float voltage);
void LCD_update(void);
void LCD_set_temp(float voltage);
