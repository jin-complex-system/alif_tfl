#ifndef APP_H
#define APP_H

typedef enum {
	APP_STATE_INIT = 0,         // initial state
	APP_STATE_CHECK_BUTTON,
	APP_STATE_SET_DEFAULT_BUFFER,
	APP_STATE_READ_SD_CARD,
	APP_STATE_SAVE_SD_CARD,
	APP_STATE_INFERENCE,
	APP_STATE_PROCESS_INFERENCE,
} APP_STATE;

/**
 * Setup application
 */
void
app_setup(void);

/**
 * Main application loop
 */
void
app_main_loop(void);

#endif // APP_H