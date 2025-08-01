/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

__STACK_SIZE    = 0x00002000;
__HEAP_SIZE     = 0x00004000;
__APP_HEAP_SIZE = 0x00004000;
/*
__ROM_BASE      = 0x80200000;
__ROM_SIZE      = 0x00200000;
*/
__ROM_BASE      = 0x80000000;
__ROM_SIZE      = 0x0047F000;

TGU_BLOCK_SIZE   = 16384;

/*
 * Note:
 * The empty regions(eg SRAM1) are ON by default, but may have been released
 * by some example applications. So make sure your application requests
 * power for these regions before adding contents.
 */

MEMORY
{
  ITCM  (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00100000
  SRAM0 (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00400000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
  MRAM  (rx)  : ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
}

ENTRY(Reset_Handler)

SECTIONS
{
  .startup.at_mram : ALIGN(16)
  {
    KEEP(*(.vectors))

    *startup_*(.text .rodata*)
    *system_*(.text .rodata*)
    *system_utils*(.text .rodata*)
    *pm*(.text .rodata*)
    *tgu_*(.text .rodata*)
    *mpu_*(.text .rodata*)
    *tcm_partition*(.text .rodata*)

    *lv_*(.text .rodata*)
    *libg.a:*printf*(.text* .rodata*)
    *flatbuffer*.obj(.text .rodata*)
    *CMSIS*.obj(.text .rodata*)

    *liblvgl.a:*(.text .rodata*)
    *libArm-2D.a:*(.text* .rodata*)
    *libcmsis_device.a:*(.text* .rodata*)
    *libensemble_services.a:*(.text* .rodata*)
    *liblcd_*.a:*(.text* .rodata*)
    *libimage_*.a:*(.text* .rodata*)
    *libcommon_api.a:*(.text* .rodata*)
    *libalif_ui_api.a:*(.text* .rodata*)
    *libg.a:*printf*(.text* .rodata*)
    *libstdc++.a:cp-demangle.o(.text* .rodata*)
    *libtensorflow-microlite.a:arm_*_s4.o(.text* .rodata*)
    *libtensorflow-microlite.a:arm_*_s16.o(.text* .rodata*)

    *(startup_ro_data)

    *(.text*)

    . = ALIGN(16);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(16);
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;

    . = ALIGN(16);
  } > MRAM

  .copy.table : ALIGN(16)
  {
    __copy_table_start__ = .;

    /* Text in ITCM */
    LONG ( LOADADDR(.text.itcm) )
    LONG ( ADDR(.text.itcm) )
    LONG ( SIZEOF(.text.itcm)/4 )

    /* Data in DTCM */
    LONG ( LOADADDR(.data.at_dtcm) )
    LONG ( ADDR(.data.at_dtcm) )
    LONG ( SIZEOF(.data.at_dtcm)/4 )

    /* Code in ITCM */
    LONG ( LOADADDR(.code.at_itcm) )
    LONG ( ADDR(.code.at_itcm) )
    LONG ( SIZEOF(.code.at_itcm)/4 )
    __copy_table_end__ = .;
    . = ALIGN(16);
  } > MRAM

  .zero.table : ALIGN(16)
  {
    __zero_table_start__ = .;
    LONG (ADDR(.bss))
    LONG (SIZEOF(.bss)/4)
    LONG (ADDR(.bss.at_sram0))
    LONG (SIZEOF(.bss.at_sram0)/4)
    __zero_table_end__ = .;
    . = ALIGN(16);
  } > MRAM

  .code.at_itcm : ALIGN(16)
  {
    . = ALIGN(16);
  } > ITCM AT > MRAM

  .vectors.itcm (NOLOAD) : ALIGN(2048)
  {
    *(.bss.noinit.ram_vectors)
  } > ITCM

  .text.itcm : ALIGN(16)
  {
    /* ITCM has only fast LVGL and RAM vectors */
    *(fast_code)
    *(.text ethosu_irq_handler)

  } > ITCM

  .data.at_dtcm : ALIGN(16)
  {
    *(vtable)
    *(.data)
    *(.data*)
    *arm_common_tables*(.data* .rodata*)

    KEEP(*(.jcr*))

    . = ALIGN(16);
  } > DTCM AT > MRAM

  .bss.at_sram0 (NOLOAD) : ALIGN(16)
  {
    *(.bss.cache_arena)       . = ALIGN(16);
    *(.bss.audio_input_buffer)
    *(.bss.power_spectrum_buffer)
    *(.bss.mel_spectrogram_buffer)
    *(.bss.prediction_buffer)

    *(.bss.s_tensorArena)     . = ALIGN(16);
    *(.bss.s_microOpResolver) . = ALIGN(16);

    *(.bss.sd_card)
    *(.bss.ethosu_drv) . = ALIGN(16);
    *(.bss.g_fileObject)
  } > SRAM0


  .bss.itcm (NOLOAD) : ALIGN(16)
  {
  } > ITCM

  .bss (NOLOAD) : ALIGN(8)
  {
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)

    . = ALIGN(8);
    __bss_end__ = .;
  } > SRAM0
  # } > DTCM /* Original */

  .nonsecure0_region (NOLOAD) : ALIGN(16)
  {
    . = ALIGN(TGU_BLOCK_SIZE);
    __ns_region_0_start = .;
    * (sd_dma_buf)
    . = ALIGN(TGU_BLOCK_SIZE);
    __ns_region_0_end = .;
  } > DTCM

  .__app_heap (NOLOAD) : ALIGN(16)
  {
     . = ALIGN(16);
     __RAM_segment_used_end__ = .;
     . = . + __APP_HEAP_SIZE;
     . = ALIGN(16);
   } > DTCM

  .heap (NOLOAD) : ALIGN(16)
  {
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > DTCM

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (NOLOAD) : ALIGN(8)
  {
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(16);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);

  .readonly.at_mram : ALIGN(16)
  {
    /* Use wildcards to mop up any read-only not directed to TCM */
    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(16);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(16);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(16);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.text*)
    *(.rodata*)
    *(nn_model) . = ALIGN(16);
    *(labels)
    *(ifm)

    . = ALIGN(16);
  } > MRAM

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}