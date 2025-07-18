#include "sd_card.h"

#include <assert.h>
#include <string.h>

/// FatFS
#include <ff.h>
#include <diskio.h>
#include <se_services_port.h>
#include <pinconf.h>

// #include "sdmmc_config.h"
#include <board.h>
#include <stdio.h>

/// File system object
static
FATFS
sd_card __attribute__((section("sd_dma_buf"))) __attribute__((aligned(512))); 

/// @brief  File object
static
FIL
g_fileObject;

/// @brief  Checks if SD card has been initialised
static
bool
sd_card_initialise = false;

#define SDC_A 1
#define SDC_B 2
#define SDC_C 3
#define SDC_PINS SDC_A

#define MEDIA_NAME "/SD_DISK/"

static
bool
setup_sd_mmc_clocks(void) {
    uint32_t error_code = SERVICES_REQ_SUCCESS;
    uint32_t service_error_code;    

    error_code = SERVICES_clocks_enable_clock(
        se_services_s_handle,
        CLKEN_CLK_100M, true,
        &service_error_code);
    if (error_code) {
        printf("SE: SDMMC 100MHz clock enable = %d\n", error_code);
        return false;
    }

    error_code = SERVICES_clocks_enable_clock(
        se_services_s_handle,
        CLKEN_USB,
        true,
        &service_error_code);
    if (error_code) {
        printf("SE: SDMMC 20MHz clock enable = %d\n", error_code);
        return false;
    }

    return true;
}

static
void
setup_sd_card_pins(void) {
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
}

bool
sd_card_setup(void) {
	FRESULT error;

    if (!setup_sd_mmc_clocks()) {
        printf("Failed to setup up SD MMC clocks\r\n");
        return false;
    }

    setup_sd_card_pins();

    FRESULT fr = f_mount(&sd_card, MEDIA_NAME, 1);

    if (fr) {
        printf("media open fail status = %d...\r\n",fr);
        return false;
    }

    sd_card_initialise = true;
    return true;
}

bool
sd_card_create_directory(
		const char* directory_str,
		const uint16_t directory_str_length,
		const bool exists_ok) {
	FRESULT error;

	/// Check parameters
	assert(sd_card_initialise);
	assert(directory_str != NULL);
	assert(directory_str_length > 0);

    error = f_mkdir(directory_str);
    assert(error != FR_INVALID_NAME);
    if (error) {
        if (error == FR_EXIST) {
            printf("Directory exists.\r\n");
            return exists_ok;
        }
        else {
            printf("Make directory failed.\r\n");
            return false;
        }
    }
    return true;
}

bool
sd_card_open_directory(
	const char* directory_str,
	const uint16_t directory_str_length,
	DIR* target_directory) {
	/// Check parameters
	assert(sd_card_initialise);
	assert(directory_str != NULL);
	assert(directory_str_length > 0);

    if (f_opendir(target_directory, directory_str)) {
        printf("Open directory failed.\r\n");
        return false;
    }

    return true;
}

void
sd_card_close_directory(
		DIR* target_directory) {
	assert(target_directory != NULL);
	f_closedir(target_directory);
}

bool
sd_card_get_next_file_information(
	DIR* target_directory,
	FILINFO* file_information) {
	FRESULT error;

	/// Check parameters
	assert(sd_card_initialise);
	assert(target_directory != NULL);
	assert(file_information != NULL);

	while(true) {
		error = f_readdir(target_directory, file_information);

	    /// Reached the end
	    if ((error != FR_OK) || (file_information->fname[0U] == 0U)) {
	        return false;
	    }
	    else if (file_information->fname[0] == '.') {
	        continue;
	    }
	    else if (file_information->fattrib & AM_DIR) {
	        continue;
	    }
	    /// Filename
	    else {
	    	return true;
	    }
	}

	// Should not reach here
	assert(false);
	return false;
}

uint32_t
sd_card_read_from_file(
	void* target_buffer,
	const uint32_t max_buffer_length,
	const char* target_filepath_str) {
	uint32_t bytesRead = 0;
	FRESULT error;

	/// Check parameters
	assert(sd_card_initialise);
	assert(target_buffer != NULL);
	assert(max_buffer_length > 0);
	assert(target_filepath_str != NULL);

	/// Open the file
    error = f_open(
    		&g_fileObject,
			_T(target_filepath_str),
			FA_READ);
    if (error != FR_OK) {
    	assert(error != FR_INVALID_NAME);

    	printf("Open file failed.\r\n");
    	return bytesRead;
    }

    /// Read from file
	error = f_read(
			&g_fileObject,
			target_buffer,
			max_buffer_length,
			(UINT*)&bytesRead);
    if ((error) || (bytesRead > max_buffer_length)) {
        printf("Read file failed. \r\n");
        bytesRead = 0;
    }

    /// Close file
    error = f_close(&g_fileObject);
    if (error) {
        printf("Closing the file failed. \r\n");
        bytesRead = 0;
    }

    return bytesRead;
}

bool
sd_card_write_to_file(
		const char* filepath_str,
		const uint16_t filepath_str_length,
		const void* data_buffer,
		const uint16_t data_buffer_length,
		const bool overwrite) {
	FRESULT error;

	/// Check parameters
	assert(sd_card_initialise);
	assert(filepath_str != NULL);
	assert(filepath_str_length > 0);

    /// Create filepath
    const uint32_t filepath_length = 200u;
    char filepath[filepath_length];
    error = f_open(
    		&g_fileObject,
			_T(filepath_str),
			(FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    if (error) {
    	assert(error != FR_INVALID_NAME);

        if (error == FR_EXIST) {
            if (overwrite) {
            	printf("File exists. Exiting\r\n");
            	return false;
            }
            else {
            	printf("Overwriting existing file\r\n");
            }
        }
        else {
            printf("Open file failed.\r\n");
            return false;
        }
    }

    /// Write to the file in one shot
    UINT bytesWritten = 0;
    error = f_write(
    		&g_fileObject,
			data_buffer,
			data_buffer_length,
			&bytesWritten);
    if ((error) || (bytesWritten != data_buffer_length)) {
        printf("Write file failed. \r\n");
        return false;
    }

    /// Flush the buffer and close
    error = f_sync(&g_fileObject);
    if (error) {
        printf("Flushing file failed. \r\n");
        return false;
    }

    error = f_close(&g_fileObject);
    if (error) {
        printf("Closing the file failed. \r\n");
        return false;
    }
    return true;
}
