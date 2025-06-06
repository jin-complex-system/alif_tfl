#include <RTE_Components.h>
#include CMSIS_device_header

#include <board.h>

#include <uart_tracelib.h>
#include <fault_handler.h>
#include <se_services_port.h>

#include "app/app.h"

inline
static void uart_callback(uint32_t event) {
    // Deliverately left blank
}

int main (void) {
    // Init pinmux using boardlib
    BOARD_Pinmux_Init();

    // Use common_app_utils for printing
    tracelib_init(NULL, uart_callback);

    fault_dump_enable(true);

    /* Initialize the SE services */
    se_services_port_init();

    app_setup();
    app_main_loop();

    while (1) __WFI();
}
