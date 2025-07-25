#ifndef NPU_DEFINITIONS_HP_H
#define NPU_DEFINITIONS_HP_H

#include <M55_HP.h>
#include <M55_HP_map.h>

#define ETHOS_U_BASE_ADDR                       LOCAL_NPU_BASE      // Ethos-U NPU base address
#define ETHOS_U_IRQN                            LOCAL_NPU_IRQ_IRQn  // Ethos-U NPU Interrupt
#define ETHOS_U_SEC_ENABLED                     0                   // Ethos-U NPU Security enable
#define ETHOS_U_PRIV_ENABLED                    0                   // Ethos-U NPU Privilege enable

#define ETHOSU_TARGET_NPU_CONFIG                ethos-u55-256

#define ETHOS_U_NPU_PORTS                       2

#define ETHOS_U_MEM_BYTE_ALIGNMENT              16

#ifndef ETHOS_U_NPU_CACHE_SIZE
    #define ETHOS_U_CACHE_BUF_SZ                (393216U)  /* See vela doc for reference */
#else
    #define ETHOS_U_CACHE_BUF_SZ                ETHOS_U_NPU_CACHE_SIZE
#endif /* ETHOS_U_NPU_CACHE_SIZE */

// #ifndef ETHOS_U_NPU_ID
#define ETHOS_U_NPU_ID U55
// #endif // ETHOS_U_NPU_ID

#ifndef ETHOS_U_NPU_CONFIG_ID
#define ETHOS_U_NPU_CONFIG_ID                   H256
#endif // ETHOS_U_NPU_CONFIG_ID

#endif // NPU_DEFINITIONS_HP_H
