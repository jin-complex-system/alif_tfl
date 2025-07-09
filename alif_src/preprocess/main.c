#include <RTE_Components.h>
#include CMSIS_device_header

#include <board.h>

#include <uart_tracelib.h>
#include <fault_handler.h>
#include <se_services_port.h>

#include "app/app.h"

#include <string.h>

inline
static void uart_callback(uint32_t event) {
    // Deliverately left blank
}

#ifdef COPY_VECTORS

static VECTOR_TABLE_Type MyVectorTable[496] __attribute__((aligned (2048))) __attribute__((section (".bss.noinit.ram_vectors")));

static inline
void copy_vtor_table_to_ram()
{
    if (SCB->VTOR == (uint32_t) MyVectorTable) {
        return;
    }
    memcpy(MyVectorTable, (const void *) SCB->VTOR, sizeof MyVectorTable);
    __DMB();
    // Set the new vector table into use.
    SCB->VTOR = (uint32_t) MyVectorTable;
    __DSB();
}
#endif

int main (void) {
    copy_vtor_table_to_ram();

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
