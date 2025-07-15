#include "led.h"

#include <Driver_GPIO.h>
#include <board.h>

#define _GET_DRIVER_REF(ref, peri, chan) \
    extern ARM_DRIVER_##peri Driver_##peri##chan; \
    static ARM_DRIVER_##peri * ref = &Driver_##peri##chan;
#define GET_DRIVER_REF(ref, peri, chan) _GET_DRIVER_REF(ref, peri, chan)

GET_DRIVER_REF(gpio_r, GPIO, BOARD_LEDRGB0_R_GPIO_PORT);
GET_DRIVER_REF(gpio_b, GPIO, BOARD_LEDRGB0_B_GPIO_PORT);
GET_DRIVER_REF(gpio_g, GPIO, BOARD_LEDRGB0_G_GPIO_PORT);

void
setup_led(void) {
    gpio_r->Initialize(BOARD_LEDRGB0_R_PIN_NO, NULL);
    gpio_r->PowerControl(BOARD_LEDRGB0_R_PIN_NO, ARM_POWER_FULL);
    gpio_r->SetDirection(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio_b->Initialize(BOARD_LEDRGB0_B_PIN_NO, NULL);
    gpio_b->PowerControl(BOARD_LEDRGB0_B_PIN_NO, ARM_POWER_FULL);
    gpio_b->SetDirection(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio_g->Initialize(BOARD_LEDRGB0_G_PIN_NO, NULL);
    gpio_g->PowerControl(BOARD_LEDRGB0_G_PIN_NO, ARM_POWER_FULL);
    gpio_g->SetDirection(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
}

void
turn_on_led(
    const LED_TYPE target_led) {
    switch(target_led) {
        case LED_RED:
            gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
        break;
        case LED_BLUE:
            gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
        break;
        case LED_GREEN:
            gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
        break;
    }
}

void
turn_off_led(
    const LED_TYPE target_led) {
    switch(target_led) {
        case LED_RED:
            gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
        break;
        case LED_BLUE:
            gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
        break;
        case LED_GREEN:
            gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
        break;
    }
}

void
toggle_led(
    const LED_TYPE target_led) {
    switch(target_led) {
        case LED_RED:
            gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        break;
        case LED_BLUE:
            gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        break;
        case LED_GREEN:
            gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        break;
    }

}
