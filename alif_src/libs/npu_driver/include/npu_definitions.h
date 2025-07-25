#ifndef NPU_DEFINITIONS_H
#define NPU_DEFINITIONS_H

#include <RTE_Components.h>                 /* For CPU related defintiions */
#include CMSIS_device_header

#if CPU_ID == 2
#include "npu_definitions_hp.h"
#endif // CPU_NAME

#if CPU_ID == 3
#include "npu_definitions_he.h"
#endif // CPU_NAME

#endif // NPU_DEFINITIONS_H
