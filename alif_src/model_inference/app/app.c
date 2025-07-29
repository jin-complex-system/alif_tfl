#include "app.h"

#include <parameters.h>

#include <stdbool.h>
#include <stdio.h>

#include <sd_card.h>
#include <ff.h>
#include <led.h>
#include <inference_tf.h>
#include <inference_model.h>
#include <npu_driver.h>

void
app_setup(void) {
    printf("app_setup()\r\n");

#if defined(ARM_NPU)
    int state;
    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        printf("Something went wrong with NPU driver!\r\n");
    }
    else {
        printf("NPU driver setup!\r\n");
    }
    
#else
    printf("ARM NPU not supported!\r\n");
#endif // ARM_NPU

    setup_led();
    inference_tf_setup();

    printf("Setting up SD card\r\n");
    if (!sd_card_setup()) {
        printf("Failed to setup SD card\r\n");
    }
    else {
        printf("Sucessfully setup SD card\r\n");
    }

    turn_on_led(LED_BLUE);
}

void
app_main_loop(void) {
    printf("app_main_loop()\r\n");

    while(true) {
        inference_tf_predict(NUM_INFERENCE_ITERATIONS);
        toggle_led(LED_GREEN);
    }
}
