#include "npu_driver.h"
#include "npu_definitions.h"

#include <RTE_Components.h>         /* For CPU related defintiions */
#include <ethosu_driver.h>          /* Arm Ethos-U driver header */

#include <stddef.h>
#include <stdio.h>

#define CACHE_BUF_SECTION           section(".bss.NoInit.ethos_u_cache")
#define CACHE_BUF_ATTRIBUTE         __attribute__((aligned(ETHOS_U_MEM_BYTE_ALIGNMENT), CACHE_BUF_SECTION))

#if defined(ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0)
static uint8_t cache_arena[ETHOS_U_CACHE_BUF_SZ] CACHE_BUF_ATTRIBUTE;
#else  /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
static uint8_t *cache_arena = NULL;
#endif /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */

struct ethosu_driver ethosu_drv __attribute__((aligned(ETHOS_U_MEM_BYTE_ALIGNMENT))); /* Default Ethos-U device driver */

static uint8_t *get_cache_arena() {
    return cache_arena;
}

static size_t get_cache_arena_size() {
#if defined(ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0)
    return sizeof(cache_arena);
#else  /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
    return 0;
#endif /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
}

/**
 * @brief   Defines the Ethos-U interrupt handler: just a wrapper around the default
 *          implementation.
 **/
static inline
void arm_ethosu_npu_irq_handler(void) {
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

int arm_ethosu_npu_init(void) {
    printf("arm_ethosu_npu_init()\r\n");

    int err = 0;

    /* Initialise Ethos-U device */
    void* const ethosu_base_address = (void *)(ETHOS_U_BASE_ADDR);
    printf("Initialising Ethos-U device@0x%p\r\n", (uint32_t)(ETHOS_U_BASE_ADDR));

    err = ethosu_init(
        &ethosu_drv,         /* Ethos-U driver device pointer */
        ethosu_base_address, /* Ethos-U NPU's base address. */
        get_cache_arena(),   /* Pointer to fast mem area - NULL for U55. */
        get_cache_arena_size(), /* Fast mem region size. */
        ETHOS_U_SEC_ENABLED,    /* Security enable. */
        ETHOS_U_PRIV_ENABLED); /* Privilege enable. */

    if (0 != err) {
        printf("Failed to initialise Ethos-U device\r\n");
        return err;
    }

    printf("Ethos-U device initialised\r\n");

    /* Get Ethos-U version */
    struct ethosu_driver_version driver_version;
    struct ethosu_hw_info hw_info;

    ethosu_get_driver_version(&driver_version);
    ethosu_get_hw_info(&ethosu_drv, &hw_info);

    printf("Ethos-U version info:\r\n");
    printf("\tArch:       v%u.%u.%u\r\n",
         hw_info.version.arch_major_rev,
         hw_info.version.arch_minor_rev,
         hw_info.version.arch_patch_rev);
    printf("\tDriver:     v%u.%u.%u\r\n",
         driver_version.major,
         driver_version.minor,
         driver_version.patch);
    printf("\tMACs/cc:    %u\r\n", (uint32_t)(1 << hw_info.cfg.macs_per_cc));
    printf("\tCmd stream: v%u\r\n", hw_info.cfg.cmd_stream_version);

    // TODO: Fix NPU IRQ
    printf("NPU IRQ needs to be fixed!\r\n");
    /// Initialise the IRQ
    printf("Set EthosU IRQ#: %u, Handler: 0x%p\r\n",
        ETHOS_U_IRQN, arm_ethosu_npu_irq_handler);

    /// Register the EthosU IRQ handler in our vector table.
    /// Note, this handler comes from the EthosU driver
    NVIC_SetVector(ETHOS_U_IRQN, (uint32_t)arm_ethosu_npu_irq_handler);
    
    printf("NVIC_SetVector() done\r\n");

    /// Enable the IRQ
    NVIC_EnableIRQ(ETHOS_U_IRQN);
    NVIC_SetPriority(ETHOS_U_IRQN, 0x60);

    printf("Successfully set EthosU IRQ#: %u, Handler: 0x%p\r\n",
          ETHOS_U_IRQN, arm_ethosu_npu_irq_handler);

    return 0;
}

