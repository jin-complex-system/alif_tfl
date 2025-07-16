#include "app.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <sd_card.h>
#include <ff.h>
#include <led.h>
#include <inference_tf.h>
#include <npu_driver.h>

#include <parameters.h>

#define ADD_HARDWARE_CODE 1
#define ADD_PREPROCESS_CODE 1

/// Audio DSP
#include <hann_window_scale_2048.h>
#include <audio_dsp_fft.h>
#include <power_spectrum.h>
#include <mel_spectrogram.h>
#include <power_to_decibel.h>

static APP_STATE
current_state = APP_STATE_INIT;

#ifdef ADD_PREPROCESS_CODE
/// Audio DSP
#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH];

float
power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH] __attribute__((aligned(16)));;

float
mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));;
#endif // ADD_PREPROCESS_CODE

#define PREDICTION_BUFFER_LENGTH        NUM_CLASSES
inference_output_data_type
prediction_buffer[PREDICTION_BUFFER_LENGTH] __attribute__((aligned(16)));

static inline
void
preprocess_buffer_measure_individual(
    const uint16_t num_iterations) {
    #ifdef ADD_PREPROCESS_CODE

    // const uint16_t total_iterations = num_iterations * NUM_SECONDS_DESIRED_AUDIO;
    const uint16_t total_iterations = num_iterations;

    /// Measure compute_power_spectrum_audio_samples()
    {
        turn_on_led(LED_GREEN);
        for (uint16_t iterator = 0; iterator < total_iterations; iterator++) {
            float max_mel = 1e-16f;

            for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
                const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;

                compute_power_spectrum_audio_samples(
                    &audio_input_buffer[audio_iterator],
                    AUDIO_FRAME_LENGTH,
                    power_spectrum_buffer,
                    POWER_SPECTRUM_BUFFER_LENGTH,
                    NULL,
                    0u,
                    HANN_WINDOW_SCALE_2048_BUFFER,
                    HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
                );
            }
        }
        turn_off_led(LED_GREEN);
    }


    /// Measure compute_power_spectrum_into_mel_spectrogram()
    {
        turn_on_led(LED_BLUE);
        for (uint16_t iterator = 0; iterator < total_iterations; iterator++) {
            float max_mel = 1e-16f;

            for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
                const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;

                const float temp_max = compute_power_spectrum_into_mel_spectrogram_raw(
                    &power_spectrum_buffer[0],
                    POWER_SPECTRUM_BUFFER_LENGTH,
                    N_FFT,
                    SAMPLING_RATE_PER_SECOND,
                    8000u,
                    &mel_spectrogram_buffer[frame_iterator * N_MELS],
                    N_MELS,
                    (float*)audio_input_buffer,
                    AUDIO_FRAME_LENGTH / (sizeof(float))
                );

                if (temp_max > max_mel) {
                    max_mel = temp_max;
                }
            }
        }
        turn_off_led(LED_BLUE);
    }

    /// Measure convert_power_to_decibel()
    {
        turn_on_led(LED_RED);
        for (uint16_t iterator = 0; iterator < total_iterations; iterator++) {
            float max_mel = 25.0f;

            convert_power_to_decibel_and_scale(
                mel_spectrogram_buffer,
                (uint8_t*)mel_spectrogram_buffer,
                N_MELS * NUM_FRAMES,
                max_mel);
        }
        turn_off_led(LED_RED);
    }
    #endif // ADD_PREPROCESS_CODE
}

static inline
void
preprocess_buffer(const uint16_t num_iterations) {
    #ifdef ADD_PREPROCESS_CODE

    // const uint16_t total_iterations = num_iterations * NUM_SECONDS_DESIRED_AUDIO;
    const uint16_t total_iterations = num_iterations;

    /// Compute mel spectrogram
    for (uint16_t iterator = 0; iterator < total_iterations; iterator++) {
        float max_mel = 1e-16f;

        for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
            const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;

            compute_power_spectrum_audio_samples(
                &audio_input_buffer[audio_iterator],
                AUDIO_FRAME_LENGTH,
                power_spectrum_buffer,
                POWER_SPECTRUM_BUFFER_LENGTH,
                NULL,
                0u,
                HANN_WINDOW_SCALE_2048_BUFFER,
                HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
            );

            const float temp_max = compute_power_spectrum_into_mel_spectrogram_raw(
                &power_spectrum_buffer[0],
                POWER_SPECTRUM_BUFFER_LENGTH,
                N_FFT,
                SAMPLING_RATE_PER_SECOND,
                8000u,
                &mel_spectrogram_buffer[frame_iterator * N_MELS],
                N_MELS,
                (float*)audio_input_buffer,
                AUDIO_FRAME_LENGTH / (sizeof(float))
            );

            if (temp_max > max_mel) {
                max_mel = temp_max;
            }
        }

        convert_power_to_decibel_and_scale(
            mel_spectrogram_buffer,
            (uint8_t*)mel_spectrogram_buffer,
            N_MELS * NUM_FRAMES,
            max_mel);
    }

    toggle_led(LED_GREEN);
    #endif // ADD_PREPROCESS_CODE
}

void
app_setup(void) {
    printf("app_setup()\r\n");

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

#ifdef ADD_HARDWARE_CODE
    setup_led();
#endif // ADD_HARDWARE_CODE

    inference_tf_setup();

#ifdef ADD_HARDWARE_CODE
    printf("Setting up SD card\r\n");
    if (!sd_card_setup()) {
        printf("Failed to setup SD card\r\n");
    }
    else {
        printf("Sucessfully setup SD card\r\n");
    }
#endif // ADD_HARDWARE_CODE
    
#ifdef ADD_PREPROCESS_CODE
    initialise_power_spectrum(N_FFT);

    printf("Mel Spectrogram length for %lu s: %lu\r\n", NUM_SECONDS_AUDIO, MEL_SPECTROGRAM_BUFFER_LENGTH);
#if NUM_SECONDS_DESIRED_AUDIO == NUM_SECONDS_AUDIO
    printf("num frames: %lu\r\n", NUM_FRAMES);
#else
    printf("num frames (scaled) desired seconds: %lu\r\n", (NUM_FRAMES * NUM_SECONDS_DESIRED_AUDIO / NUM_SECONDS_AUDIO));
#endif // NUM_SECONDS_DESIRED_AUDIO
#endif // ADD_PREPROCESS_CODE

#ifdef ADD_HARDWARE_CODE
    turn_on_led(LED_RED);
#endif // ADD_HARDWARE_CODE

    current_state = APP_STATE_INIT;
}

void
app_main_loop(void) {
    printf("app_main_loop()\r\n");

    /// Constants
    const char INPUT_DIRECTORY[] = "ALIF";
    const char OUTPUT_DIRECTORY[] = "outA";

    /// Variables
    DIR read_directory;
    bool read_sd_card = false;
    bool success;
    FILINFO current_file_info;

    current_state = APP_STATE_INIT;
    printf("Begin while(1)\r\n");
    while(true) {
		switch(current_state) {
            case APP_STATE_INIT:
                printf("APP_STATE_INIT");
                read_sd_card = false;
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
                    /// Use mel spectrogram buffer as a temporary file
                    char* filepath = (char*)(&mel_spectrogram_buffer[0]);
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

                    // Copy audio into buffer
                    uint32_t length_read = sd_card_read_from_file(
                            audio_input_buffer,
                            sizeof(audio_input_buffer),
                            filepath);
                    printf("Length read: %u\r\n", length_read);
                    // printf("I read: %c\r\n", audio_input_buffer[0]);
                    assert(length_read > 0);
                    // assert(length_read == sizeof(audio_input_buffer));

                    current_state = APP_STATE_PREPROCESS;
                }
                assert(current_state != APP_STATE_READ_SD_CARD);
                break;
            case APP_STATE_PREPROCESS:
                printf("APP_STATE_PREPROCESS\r\n");
                #define NUM_ITERATIONS 1

                memset(power_spectrum_buffer, 0, sizeof(power_spectrum_buffer));
                memset(mel_spectrogram_buffer, 0, sizeof(mel_spectrogram_buffer));
                memset(prediction_buffer, 0, sizeof(prediction_buffer));

                printf("Begin preprocessing %u iterations\r\n", NUM_ITERATIONS);

                preprocess_buffer(NUM_ITERATIONS);
                // preprocess_buffer_measure_individual(NUM_ITERATIONS);

                current_state = APP_STATE_INFERENCE;
                break;
            case APP_STATE_INFERENCE:
                printf("APP_STATE_INFERENCE\r\n");
                inference_tf_set_input(
                    (inference_input_data_type*)mel_spectrogram_buffer,
                    MEL_SPECTROGRAM_BUFFER_LENGTH
                );
                inference_tf_predict();

                current_state = APP_STATE_PROCESS_INFERENCE;
                break;
            case APP_STATE_PROCESS_INFERENCE:
                inference_tf_get_output(
                    prediction_buffer,
                    PREDICTION_BUFFER_LENGTH
                );

                int8_t best_result = -110;
                uint16_t best_class_id = PREDICTION_BUFFER_LENGTH;

                for (uint16_t class_iterator = 0; class_iterator < PREDICTION_BUFFER_LENGTH; class_iterator++) {
                    if (prediction_buffer[class_iterator] > best_result) {
                        best_class_id = class_iterator;
                        best_result = prediction_buffer[class_iterator];
                    }
                }

                printf("Best class: %u, with result %i\r\n", best_class_id, (int16_t)best_result);
                current_state = APP_STATE_SAVE_SD_CARD;
                break;
            case APP_STATE_SAVE_SD_CARD:
                printf("APP_STATE_SAVE_SD_CARD\r\n");
                if (read_sd_card) {
                    printf("Open %s\r\n", current_file_info.fname);

                    // DIR write_directory;

                    // success =
                    // sd_card_open_directory(
                    //     OUTPUT_DIRECTORY,
                    //     sizeof(OUTPUT_DIRECTORY),
                    //     &write_directory);
                    // assert(success);

                    success =
                    sd_card_write_to_file(
                        current_file_info.fname,
                        strlen(current_file_info.fname),
                        OUTPUT_DIRECTORY,
                        sizeof(OUTPUT_DIRECTORY),
                        prediction_buffer,
                        sizeof(prediction_buffer),
                        true
                    );
                    printf("Saved prediction to SD card output directory\r\n");
                    assert(success);

                    // sd_card_close_directory(&write_directory);
                }

                current_state = APP_STATE_CHECK_BUTTON;
                break;
            default:
                printf("default\r\n");
                current_state = APP_STATE_INIT;
                break;
        }
    }
}
