/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include <time.h>

#include <RTE_Components.h>
#include CMSIS_device_header

#include <stdio.h>
#include <string.h>

#include <board.h>
#include <uart_tracelib.h>
#include <fault_handler.h>
#include <se_services_port.h>

#include "unit_tests/utils/test_square_root_approximation.h"
#include "unit_tests/app/test_app_preprocess.h"

inline
static void uart_callback(uint32_t event)
{
    // Deliberately left blank
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

int main (void)
{
    copy_vtor_table_to_ram();

    // Init pinmux using boardlib
    BOARD_Pinmux_Init();

    // Use common_app_utils for printing
    tracelib_init(NULL, uart_callback);

    fault_dump_enable(true);

    /* Initialize the SE services */
    se_services_port_init();

    BOARD_LED2_Control(BOARD_LED_STATE_HIGH);

    printf("\r\nHello World!\r\n");

    test_square_root_approximation();
    test_app_preprocess();

    BOARD_LED2_Control(BOARD_LED_STATE_LOW);

    while (1) __WFE();
}
