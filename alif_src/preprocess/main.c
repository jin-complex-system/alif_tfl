#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_GPIO.h"
#include "board.h"
#include "stdio.h"
#include "string.h"
#include "parameters.h"

#include "uart_tracelib.h"
#include "fault_handler.h"

static void uart_callback(uint32_t event) {
}

/// FatFS
#include "ff.h"
#include "diskio.h"
#include "se_services_port.h"
#include "pinconf.h"

/// Audio DSP
#include "hann_window_scale_1024.h"
#include "audio_dsp_fft.h"
#include "power_spectrum.h"
#include "mel_spectrogram.h"

#define _GET_DRIVER_REF(ref, peri, chan) \
    extern ARM_DRIVER_##peri Driver_##peri##chan; \
    static ARM_DRIVER_##peri * ref = &Driver_##peri##chan;
#define GET_DRIVER_REF(ref, peri, chan) _GET_DRIVER_REF(ref, peri, chan)

GET_DRIVER_REF(gpio_b, GPIO, BOARD_LEDRGB0_B_GPIO_PORT);
GET_DRIVER_REF(gpio_r, GPIO, BOARD_LEDRGB0_R_GPIO_PORT);

#define SDC_A 1
#define SDC_B 2
#define SDC_C 3
#define SDC_PINS SDC_A

#define MEDIA_NAME "/SD_DISK/"

#define TEST_FILE "/TestFile34.txt"
/* Define Test Requirement <Test File Name> */
#define FILE_CREATE_TEST TEST_FILE
#define FILE_READ_TEST TEST_FILE
#define FILE_WRITE_TEST TEST_FILE

#define K (1024)

/* Tasks Pool size, stack size, and pointers */
#define STACK_POOL_SIZE (40*K)
#define SD_STACK_SIZE (10*K)
#define SD_BLK_SIZE 512
#define NUM_BLK_TEST 1
#define SD_TEST_ITTR_CNT 10

uint32_t count1, count2, total_cnt=0;
uint32_t service_error_code;
uint32_t error_code = SERVICES_REQ_SUCCESS;

/* Buffer for FileX FF_Disk_t sector cache. This must be large enough for at least one sector , which are typically 512 bytes in size. */
unsigned char filebuffer[(SD_BLK_SIZE*NUM_BLK_TEST) + 1] __attribute__((section("sd_dma_buf"))) __attribute__((aligned(512)));
FATFS sd_card __attribute__((section("sd_dma_buf"))) __attribute__((aligned(512)));
FIL test_file;

/// Audio DSP
#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
input_buffer[INPUT_BUFFER_LENGTH];

float
power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH];

float
mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];

void
sd_card_fatfs(void) {
    unsigned long int   actual;
    unsigned long int   startCnt, EndCnt;
    FRESULT fr;
    UINT br,bw;
    FILE *fp = NULL;

    printf("SD card configuration...\r\n");

#if (SDC_PINS == SDC_A)
    pinconf_set(PORT_7, PIN_0, PINMUX_ALTERNATE_FUNCTION_6, PADCTRL_READ_ENABLE); //cmd
    pinconf_set(PORT_7, PIN_1, PINMUX_ALTERNATE_FUNCTION_6, PADCTRL_READ_ENABLE); //clk
    pinconf_set(PORT_5, PIN_0, PINMUX_ALTERNATE_FUNCTION_7, PADCTRL_READ_ENABLE); //d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_5, PIN_1, PINMUX_ALTERNATE_FUNCTION_7, PADCTRL_READ_ENABLE); //d1
    pinconf_set(PORT_5, PIN_2, PINMUX_ALTERNATE_FUNCTION_7, PADCTRL_READ_ENABLE); //d2
    pinconf_set(PORT_5, PIN_3, PINMUX_ALTERNATE_FUNCTION_6, PADCTRL_READ_ENABLE); //d3
#endif // RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE

#elif (SDC_PINS == SDC_B)
    pinconf_set(PORT_14, PIN_0, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //cmd
    pinconf_set(PORT_14, PIN_1, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //clk
    pinconf_set(PORT_14, PIN_2, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //rst
    pinconf_set(PORT_13, PIN_0, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_13, PIN_1, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //d1
    pinconf_set(PORT_13, PIN_2, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //d2
    pinconf_set(PORT_13, PIN_3, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //d3
#endif // RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE

#elif (SDC_PINS == SDC_C)
    pinconf_set(PORT_7, PIN_0, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //cmd
    pinconf_set(PORT_7, PIN_1, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //clk
    pinconf_set(PORT_5, PIN_0, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_5, PIN_1, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //d1
    pinconf_set(PORT_5, PIN_2, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //d2
    pinconf_set(PORT_5, PIN_3, PINMUX_ALTERNATE_FUNCTION_0, PADCTRL_READ_ENABLE | PADCTRL_DRIVER_DISABLED_BUS_REPEATER); //d3
#endif // RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_9, PIN_0, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //cmd
    pinconf_set(PORT_9, PIN_1, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //clk
    pinconf_set(PORT_8, PIN_0, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_8, PIN_1, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //d1
    pinconf_set(PORT_8, PIN_2, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //d2
    pinconf_set(PORT_8, PIN_3, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA); //d3
#endif // RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE

#elif (SDC_PINS == SDC_D)
    pinconf_set(PORT_9, PIN_0, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //cmd
    pinconf_set(PORT_9, PIN_1, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //clk
    //pinconf_set(PORT_9, PIN_2, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //rst
    pinconf_set(PORT_8, PIN_0, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_8, PIN_1, PINMUX_ALTERNATE_FUNCTION_4, PADCTRL_READ_ENABLE); //d1
    pinconf_set(PORT_8, PIN_2, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //d2
    pinconf_set(PORT_8, PIN_3, PINMUX_ALTERNATE_FUNCTION_5, PADCTRL_READ_ENABLE); //d3
#endif // RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE

#else
    #error "Invalid SDMMC Pins defined...\r\n"
#endif // SDC_PINS

    printf("Attempting to mount SD\r\n");

    /* Open the SD disk. and initialize SD controller */
    fr = f_mount(&sd_card, MEDIA_NAME, 1);

    /* Check the media open status.  */
    if (fr) {
        printf("media open fail status = %d...\r\n",fr);
        while(1);
    }
    printf("SD Mounted Successfully...\r\n");

#ifdef FILE_CREATE_TEST
    /* Create a file in the root directory.  */
    fr =  f_open(&test_file, FILE_CREATE_TEST, FA_CREATE_ALWAYS | FA_WRITE);

    if (fr)
    {
        printf("File open status: %d\r\n",fr);
        /* Error opening file, break the loop.  */
        while(1);
    }


#elif defined(FILE_READ_TEST)

    /* Open the test file.  */
    fr = f_open(&test_file, FILE_READ_TEST, FA_READ);

    /* Check the file open status.  */
    if (fr)
    {
        printf("File open status: %d\r\n",fr);
        /* Error opening file, break the loop.  */
        while(1);
    }

    printf("Reading Data from File...%s\r\n",FILE_READ_TEST);
    memset(filebuffer,'\0',sizeof(filebuffer));

    while(1)
    {

        fr =  f_read(&test_file, filebuffer, (SD_BLK_SIZE * NUM_BLK_TEST), &br);

        /* Check the file read status.  */
        if (fr)
        {
            /* Error performing file read, break the loop.  */
            printf("File read status: %d\r\n",fr);
            break;
        }

        if( f_eof(&test_file) )
        {
            printf("End of File Reached...\r\n");
            break;
        }

        printf("size = %u\n %s\n",br, (const char *)filebuffer);

    }

#elif defined(FILE_WRITE_TEST)

    /* Open the test file.  */
    fr = f_open(&test_file, FILE_WRITE_TEST, FA_CREATE_ALWAYS | FA_WRITE);

    /* Check the file open status.  */
    if (fr)
    {
        printf("File open status: %d\r\n",fr);
        /* Error opening file, break the loop.  */
        while(1);
    }

    printf("Writing Data in File...%s\r\n",FILE_WRITE_TEST);
    memset(filebuffer, '\0', sizeof(filebuffer));

    for(int i = 0; i<SD_TEST_ITTR_CNT; i++)
    {

        memset(filebuffer, 'F', (SD_BLK_SIZE * NUM_BLK_TEST));

        /* Write a string to the test file.  */
        fr =  f_write(&test_file, (void *)filebuffer, (SD_BLK_SIZE * NUM_BLK_TEST), &bw);

        /* Check the file write status.  */
        if (fr)
        {
            printf("ittr: %d File write status: %d\r\n",i, fr);
            break;
        }
    }
#else
#error "No Test Defined...\r\n"

#endif

    printf("Closing File...%s\r\n",TEST_FILE);

    /* Close the test file.  */
    fr = f_close(&test_file);

    /* Check the file close status.  */
    if (fr)
    {
        printf("File close status: %d\r\n",fr);
        /* Error closing the file, break the loop.  */
        while(1);
    }

    printf("File R/W Test Completed!!!\r\n");

    error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_CLK_100M, false, &service_error_code);
    if(error_code)
    {
        printf("SE: SDMMC 100MHz clock disable = %d\r\n", error_code);
        return;
    }

    error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_USB, false, &service_error_code);
    if(error_code)
    {
        printf("SE: SDMMC 20MHz clock disable = %d\r\n", error_code);
        return;
    }
}

void
preprocess_buffer(void) {
	/// Compute mel spectrogram
    #if NUM_SECONDS_AUDIO == NUM_SECONDS_DESIRED_AUDIO
    const uint16_t num_iterations = NUM_SECONDS_AUDIO;
    #else
    const uint16_t num_iterations = NUM_SECONDS_DESIRED_AUDIO / NUM_SECONDS_AUDIO;
    #endif // NUM_SECONDS_AUDIO

    for (uint16_t iterator = 0; iterator < num_iterations; iterator++) {
        for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
            const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;

            compute_power_spectrum_audio_samples(
                &input_buffer[audio_iterator],
                AUDIO_FRAME_LENGTH,
                power_spectrum_buffer,
                POWER_SPECTRUM_BUFFER_LENGTH,
                NULL,
                0u,
                0.0f,
                HANN_WINDOW_SCALE_1024_BUFFER,
                HANN_WINDOW_SCALE_1024_BUFFER_LENGTH
            );

            compute_power_spectrum_into_mel_spectrogram(
                &power_spectrum_buffer[0],
                POWER_SPECTRUM_BUFFER_LENGTH,
                &mel_spectrogram_buffer[frame_iterator * N_MELS],
                N_FFT,
                SAMPLING_RATE_PER_SECOND,
                N_MELS
            );
        }
    }
}

int main (void) {
    // Init pinmux using boardlib
    BOARD_Pinmux_Init();

    // Use common_app_utils for printing
    tracelib_init(NULL, uart_callback);

    fault_dump_enable(true);

    /* Initialize the SE services */
    se_services_port_init();

    // TODO: Enable SDMMC Clocks
    error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_CLK_100M, true, &service_error_code);
    if(error_code)
    {
        printf("SE: SDMMC 100MHz clock enable = %d\n", error_code);
        return 0;
    }

    error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_USB, true, &service_error_code);
    if(error_code)
    {
        printf("SE: SDMMC 20MHz clock enable = %d\n", error_code);
        return 0;
    }

    gpio_b->Initialize(BOARD_LEDRGB0_B_PIN_NO, NULL);
    gpio_b->PowerControl(BOARD_LEDRGB0_B_PIN_NO, ARM_POWER_FULL);
    gpio_b->SetDirection(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio_r->Initialize(BOARD_LEDRGB0_R_PIN_NO, NULL);
    gpio_r->PowerControl(BOARD_LEDRGB0_R_PIN_NO, ARM_POWER_FULL);
    gpio_r->SetDirection(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    printf("Hello World\r\n");

    gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
    sd_card_fatfs();

    initialise_power_spectrum(N_FFT);

    #if NUM_SECONDS_DESIRED_AUDIO == NUM_SECONDS_AUDIO
    printf("num frames: %lu\r\n", NUM_FRAMES);
    #else
    printf("num frames (scaled) desired seconds: %lu\r\n", (NUM_FRAMES * NUM_SECONDS_DESIRED_AUDIO / NUM_SECONDS_AUDIO));
    #endif // NUM_SECONDS_DESIRED_AUDIO

    while(1) {
        preprocess_buffer();
        gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        // printf("Done preprocessing\r\n");
    }

    while (1) __WFI();
}
