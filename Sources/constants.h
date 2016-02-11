#define DISPLAY_REFRESH_DELAY 1
#define TEMP_POLL_DELAY 100

#define TEST_DELAY 100

struct tick_counter {
	int display;
	int temp;
	int alarm;
	int test;
} tick_counts;
