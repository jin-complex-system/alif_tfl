#include "app.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <parameters.h>

#include <app_preprocess.h>

#include <sd_card.h>
#include <ff.h>
#include <led.h>
#include <inference_tf.h>
#include <npu_driver.h>

static APP_STATE
current_state = APP_STATE_INIT;

/// Audio DSP
#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH] __attribute__((aligned(16)));

inference_input_data_type
scale_mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

#define PREDICTION_BUFFER_LENGTH        NUM_CLASSES
inference_output_data_type
prediction_buffer[PREDICTION_BUFFER_LENGTH] __attribute__((aligned(16)));

void
app_setup(void) {
    printf("app_setup()\r\n");

    volatile uint32_t i=0x01234567;
    volatile uint8_t endian_indicator = (*((uint8_t*)(&i)));

    if (endian_indicator == 0x67) {
        // Decimel: 103
        printf("Little endian: %lu\r\n", (uint16_t)endian_indicator);
    }
    else {
        printf("Big endian: %lu\r\n", (uint16_t)endian_indicator);
    }

#if defined(ARM_NPU)
    int state;
    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        printf("Something went wrong with NPU driver!\r\n");
    }
    else {
        printf("NPU driver setup!\r\n");
    }
    
#else
    printf("ARM NPU not supported!\r\n");
#endif // ARM_NPU

    setup_led();
    inference_tf_setup();

    printf("Setting up SD card\r\n");
    if (!sd_card_setup()) {
        printf("Failed to setup SD card\r\n");
    }
    else {
        printf("Sucessfully setup SD card\r\n");
    }

#ifdef LOAD_AUDIO_AND_PREPROCESS
    setup_preprocess(N_FFT);
#endif // LOAD_AUDIO_AND_PREPROCESS

    printf("Audio input buffer length: %lu\r\n", INPUT_BUFFER_LENGTH);
    printf("Mel Spectrogram length for %lu s: %lu\r\n", NUM_SECONDS_AUDIO, MEL_SPECTROGRAM_BUFFER_LENGTH);
#if NUM_SECONDS_DESIRED_AUDIO == NUM_SECONDS_AUDIO
    printf("num frames: %lu, num_cropped_frames: %lu \r\n", NUM_FRAMES, CROPPED_NUM_FRAMES);
#else
    printf("num frames (scaled) desired seconds: %lu\r\n", (NUM_FRAMES * NUM_SECONDS_DESIRED_AUDIO / NUM_SECONDS_AUDIO));
#endif // NUM_SECONDS_DESIRED_AUDIO
    printf("Cropped mel spectrogram length: %lu\r\n", CROPPED_MEL_SPEC_BUFFER_LENGTH);

    /// Clear all the buffers
    memset(audio_input_buffer, 0, sizeof(audio_input_buffer));
    memset(scale_mel_spectrogram_buffer, 0, sizeof(scale_mel_spectrogram_buffer));
    memset(prediction_buffer, 0, sizeof(prediction_buffer));

    turn_on_led(LED_RED);

    current_state = APP_STATE_INIT;
}

void
app_main_loop(void) {
    printf("app_main_loop()\r\n");

#ifdef USE_ORBIWISE
#ifdef LOAD_AUDIO_AND_PREPROCESS
    /// Constants
    const char INPUT_DIRECTORY[] = "ow_audio";
    const char OUTPUT_DIRECTORY[] = "out_owA";
#else
    const char INPUT_DIRECTORY[] = "ow_pre";
    const char OUTPUT_DIRECTORY[] = "out_owP";
#endif // LOAD_AUDIO_AND_PREPROCESS

#else
#ifdef LOAD_AUDIO_AND_PREPROCESS
    /// Constants
    const char INPUT_DIRECTORY[] = "ub_audio";
    const char OUTPUT_DIRECTORY[] = "out_ubA";
#else
    const char INPUT_DIRECTORY[] = "ub_pre";
    const char OUTPUT_DIRECTORY[] = "ub_owP";
#endif // LOAD_AUDIO_AND_PREPROCESS
#endif // USE_ORBIWISE

    /// Variables
    DIR read_directory;
    bool read_sd_card = false;
    bool success;
    FILINFO current_file_info;
    uint64_t num_inferences = 0;
    uint32_t class_id = NUM_CLASSES;
    uint32_t length_read = 0;

    current_state = APP_STATE_INIT;
    printf("Begin while(1)\r\n");
    while(true) {
		switch(current_state) {
            case APP_STATE_INIT:
                printf("APP_STATE_INIT");
                read_sd_card = false;
                class_id = NUM_CLASSES;
                num_inferences = 0;

                current_state = APP_STATE_CHECK_BUTTON;
                break;
            case APP_STATE_CHECK_BUTTON:
                printf("APP_STATE_CHECK_BUTTON\r\n");
                /// On button press, read SD card and get the input directory
                // TODO: Implement button checking mechanism
                if (!read_sd_card) {
                    success = 
                    sd_card_open_directory(
                        INPUT_DIRECTORY,
                        sizeof(INPUT_DIRECTORY),
                        &read_directory);
                    assert(success);
                    read_sd_card = success;
                }

                if (read_sd_card) {
                    current_state = APP_STATE_READ_SD_CARD;
                }
                /// Otherwise, just inference as it is
                else {
                    current_state = APP_STATE_SET_DEFAULT_BUFFER;
                }
                break;
            case APP_STATE_SET_DEFAULT_BUFFER:
                printf("APP_STATE_SET_DEFAULT_BUFFER\r\n");
                memset(audio_input_buffer, 0, sizeof(audio_input_buffer));

                current_state = APP_STATE_PREPROCESS;
                break;
            case APP_STATE_READ_SD_CARD:
                printf("APP_STATE_READ_SD_CARD\r\n");
                assert(read_sd_card);
                class_id = NUM_CLASSES + 1;

                success = sd_card_get_next_file_information(
					&read_directory,
					&current_file_info);

                //// Reached end of directory
                if (!success) {
                    printf("End of directory\r\n");
                    read_sd_card = false;

                    sd_card_close_directory(&read_directory);
                    current_state = APP_STATE_SET_DEFAULT_BUFFER;
                }
                /// Successfully read file
                else {
                    memset(audio_input_buffer, 0, sizeof(audio_input_buffer));

                    /// Craft filepath to file
                    /// Use power spectrum buffer as a temporary file
                    char* filepath = (char*)(&power_spectrum_buffer[0]);
                    sprintf(
                        filepath,
                        "%s/%s",
                        INPUT_DIRECTORY,
                        current_file_info.fname
                    );
                    printf(
                        "Read input file: %s, with file size %lu\r\n",
                        filepath,
                        current_file_info.fsize
                    );

                    /// Set the class id from the filename
                    const char* sample_id_string = strtok(current_file_info.fname, "-");
                    const char* class_id_string = strtok(NULL, "-");

                    class_id = atoi(class_id_string);
                    assert(class_id < NUM_CLASSES);

                #ifdef LOAD_AUDIO_AND_PREPROCESS

                    // Copy audio into buffer
                    length_read = 0;
                    length_read = sd_card_read_from_file(
                            audio_input_buffer,
                            sizeof(audio_input_buffer),
                            filepath);
                    printf("Length read: %u\r\n", length_read);
                    
                    assert(length_read > 0 && length_read <= sizeof(audio_input_buffer));
                    current_state = APP_STATE_PREPROCESS;
                #else
                    length_read = 0;
                    length_read = sd_card_read_from_file(
                            scale_mel_spectrogram_buffer,
                            sizeof(scale_mel_spectrogram_buffer),
                            filepath);
                    printf("Length read: %u\r\n", length_read);

                    current_state = APP_STATE_INFERENCE;
                    assert(length_read > 0 && length_read <= sizeof(scale_mel_spectrogram_buffer));
                #endif // LOAD_AUDIO_AND_PREPROCESS
                }
                assert(current_state != APP_STATE_READ_SD_CARD);
                break;
            case APP_STATE_PREPROCESS:
                printf("APP_STATE_PREPROCESS\r\n");

#ifdef LOAD_AUDIO_AND_PREPROCESS
                uint32_t num_valid_audio_elements = length_read / sizeof(audio_data_type);
                uint32_t num_frames_to_read = (num_valid_audio_elements / HOP_LENGTH - N_FFT / HOP_LENGTH) + 1;

                /// Upper limit of number of frames
                if (num_frames_to_read > NUM_FRAMES) {
                    num_frames_to_read = NUM_FRAMES;
                }

                preprocess(
                    audio_input_buffer,
                    INPUT_BUFFER_LENGTH,
                    NUM_PREPROCESS_ITERATIONS,
                    num_frames_to_read,
                    scale_mel_spectrogram_buffer,
                    MEL_SPECTROGRAM_BUFFER_LENGTH
                );
#endif // LOAD_AUDIO_AND_PREPROCESS

                current_state = APP_STATE_INFERENCE;
                break;
            case APP_STATE_INFERENCE:
                printf("APP_STATE_INFERENCE\r\n");

                toggle_led(LED_BLUE);

                assert(CROPPED_MEL_SPEC_BUFFER_LENGTH <= MEL_SPECTROGRAM_BUFFER_LENGTH);
                inference_tf_set_input(
                    scale_mel_spectrogram_buffer,
                    CROPPED_MEL_SPEC_BUFFER_LENGTH
                );

                inference_tf_predict(NUM_INFERENCE_ITERATIONS);

                toggle_led(LED_BLUE);

                current_state = APP_STATE_PROCESS_INFERENCE;
                break;
            case APP_STATE_PROCESS_INFERENCE:
                inference_tf_get_output(
                    prediction_buffer,
                    PREDICTION_BUFFER_LENGTH
                );

                int8_t best_result = -110;
                uint16_t predicted_class_id = PREDICTION_BUFFER_LENGTH;

                for (uint16_t class_iterator = 0; class_iterator < PREDICTION_BUFFER_LENGTH; class_iterator++) {
                    if (prediction_buffer[class_iterator] > best_result) {
                        predicted_class_id = class_iterator;
                        best_result = prediction_buffer[class_iterator];
                    }
                }
                num_inferences++;

                printf(
                    "Inference iteration: %llu, Class id: %u, predicted class: %u, with result %i\r\n",
                    num_inferences,
                    class_id,
                    predicted_class_id, 
                    (int16_t)best_result);
                current_state = APP_STATE_SAVE_SD_CARD;
                break;
            case APP_STATE_SAVE_SD_CARD:

            #ifdef SAVE_RESULT_TO_SD_CARD
                printf("APP_STATE_SAVE_SD_CARD\r\n");
                if (read_sd_card) {
                    char* output_filepath = (char*)(&power_spectrum_buffer[0]);
                    sprintf(
                        output_filepath,
                        "%s/%s",
                        OUTPUT_DIRECTORY,
                        current_file_info.fname
                    );
                    printf("Open output file: %s\r\n", output_filepath);

                    // DIR write_directory;

                    // success =
                    // sd_card_open_directory(
                    //     OUTPUT_DIRECTORY,
                    //     sizeof(OUTPUT_DIRECTORY),
                    //     &write_directory);
                    // assert(success);

                    success =
                    sd_card_write_to_file(
                        output_filepath,
                        strlen(output_filepath),
                        prediction_buffer,
                        sizeof(prediction_buffer),
                        true
                    );
                    printf("Saved prediction to SD card output directory\r\n");
                    assert(success);

                    // sd_card_close_directory(&write_directory);
                }
            #endif // SAVE_RESULT_TO_SD_CARD

                current_state = APP_STATE_CHECK_BUTTON;
                break;
            default:
                printf("default\r\n");
                current_state = APP_STATE_INIT;
                break;
        }
    }
}
