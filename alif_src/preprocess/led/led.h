#ifndef LED_H
#define LED_H

typedef enum {
    LED_RED,
    LED_BLUE,
    LED_GREEN,
} LED_TYPE;

/**
 * Setup LED
 */
void
setup_led(void);

/**
 * Turn on the LED
 * 
 * @param target_led
 */
void
turn_on_led(
    const LED_TYPE target_led);

/**
 * Turn off the LED
 * 
 * @param target_led
 */
void
turn_off_led(
    const LED_TYPE target_led);

/**
 * Toggle LED
 * 
 * @param target_led
 */
void
toggle_led(
    const LED_TYPE target_led);

#endif // LED_H