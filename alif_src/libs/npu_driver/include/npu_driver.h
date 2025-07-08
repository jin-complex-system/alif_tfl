#ifndef NPU_DRIVER_H
#define NPU_DRIVER_H

#include <stdint.h>

#define ARM_NPU 1

/**
 * @brief   Initialises the Arm Ethos-U NPU
 * @return  0 if successful, error code otherwise
 **/
int
arm_ethosu_npu_init(void);

#endif // NPU_DRIVER_H
