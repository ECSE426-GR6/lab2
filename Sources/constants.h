#define DISPLAY_REFRESH_DELAY 1
#define LCD_UPDATE_DELAY 1
#define TEMP_POLL_DELAY 500
#define ALARM_SWITCH_DELAY 50

#define TEST_DELAY 100

#define ALARM_THRESHOLD 50.0f

struct tick_counter {
	int display;
	int lcd;
	int temp;
	int alarm;
	int test;
} tick_counts;
