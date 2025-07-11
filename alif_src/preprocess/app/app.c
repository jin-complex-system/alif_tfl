#include "app.h"

#include <stdio.h>

#include <sd_card.h>
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

/// SD card stuff
#define INPUT_DIRECTORY INPUT
#define OUTPUT_DIRECTORY OUTPUT
#define OVERWRITE_FILE_IS_OKAY true

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

                const float temp_max = compute_power_spectrum_into_mel_spectrogram(
                    &power_spectrum_buffer[0],
                    POWER_SPECTRUM_BUFFER_LENGTH,
                    &mel_spectrogram_buffer[frame_iterator * N_MELS],
                    N_FFT,
                    SAMPLING_RATE_PER_SECOND,
                    8000u,
                    N_MELS
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

            convert_power_to_decibel(
                mel_spectrogram_buffer,
                N_MELS * NUM_FRAMES,
                max_mel,
                TOP_DECIBEL
            );
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

            const float temp_max = compute_power_spectrum_into_mel_spectrogram(
                &power_spectrum_buffer[0],
                POWER_SPECTRUM_BUFFER_LENGTH,
                &mel_spectrogram_buffer[frame_iterator * N_MELS],
                N_FFT,
                SAMPLING_RATE_PER_SECOND,
                8000u,
                N_MELS
            );

            if (temp_max > max_mel) {
                max_mel = temp_max;
            }
        }

        convert_power_to_decibel(
            mel_spectrogram_buffer,
            N_MELS * NUM_FRAMES,
            max_mel,
            TOP_DECIBEL
        );
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

    current_state = APP_STATE_CHECK_BUTTON;
    while(true) {
		switch(current_state) {
            case APP_STATE_CHECK_BUTTON:
                // TODO: Implement button checking mechanism

                current_state = APP_STATE_SET_DEFAULT_BUFFER;
                break;
            case APP_STATE_SET_DEFAULT_BUFFER:
                memset(audio_input_buffer, 0, sizeof(audio_input_buffer));
                memset(prediction_buffer, 0, sizeof(prediction_buffer));

                current_state = APP_STATE_READ_SD_CARD;
                break;
            case APP_STATE_READ_SD_CARD:
                // TODO: Implement reading audio from SD card

                current_state = APP_STATE_PREPROCESS;
                break;
            case APP_STATE_PREPROCESS:
                #define NUM_ITERATIONS 20

                // preprocess_buffer(NUM_ITERATIONS);
                preprocess_buffer_measure_individual(NUM_ITERATIONS);
                printf("Done preprocessing %u iterations\r\n", NUM_ITERATIONS);
                
                current_state = APP_STATE_INFERENCE;
                break;
            case APP_STATE_INFERENCE:
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

                int16_t best_result = -125.0f;
                uint16_t best_class_id = PREDICTION_BUFFER_LENGTH;

                for (uint16_t class_iterator = 0; class_iterator < PREDICTION_BUFFER_LENGTH; class_iterator++) {
                    if (prediction_buffer[class_iterator] > best_result) {
                        best_class_id = class_iterator;
                        best_result = prediction_buffer[class_iterator];
                    }
                }

                printf("Best class: %u, with result %i\r\n", best_class_id, best_result);
                current_state = APP_STATE_SAVE_SD_CARD;
                break;
            case APP_STATE_SAVE_SD_CARD:
                // TODO: Implement saving results to SD card

                current_state = APP_STATE_CHECK_BUTTON;
                break;
            default:
                current_state = APP_STATE_CHECK_BUTTON;
                break;
        }
    }
}
