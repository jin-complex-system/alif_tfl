#include "app.h"

#include <stdio.h>

#include <sd_card.h>
#include <led.h>

#include <parameters.h>

/// Audio DSP
#include <hann_window_scale_1024.h>
#include <audio_dsp_fft.h>
#include <power_spectrum.h>
#include <mel_spectrogram.h>

static APP_STATE
current_state = APP_STATE_INIT;

/// SD card stuff
#define INPUT_DIRECTORY INPUT
#define OUTPUT_DIRECTORY OUTPUT
#define OVERWRITE_FILE_IS_OKAY true

/// Audio DSP
#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH];

float
power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH];

float
mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];

static
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
                &audio_input_buffer[audio_iterator],
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

void
app_setup(void) {

    setup_led();

    if (!sd_card_setup()) {
        printf("Failed to setup SD card\r\n");
    }
    
    initialise_power_spectrum(N_FFT);

#if NUM_SECONDS_DESIRED_AUDIO == NUM_SECONDS_AUDIO
    printf("num frames: %lu\r\n", NUM_FRAMES);
#else
    printf("num frames (scaled) desired seconds: %lu\r\n", (NUM_FRAMES * NUM_SECONDS_DESIRED_AUDIO / NUM_SECONDS_AUDIO));
#endif // NUM_SECONDS_DESIRED_AUDIO

    turn_on_led(LED_RED);

    current_state = APP_STATE_INIT;
}

void
app_main_loop(void) {

    while(1) {
        preprocess_buffer();
        toggle_led(LED_BLUE);
        printf("Done preprocessing\r\n");
    }

    // TODO: Enter into main loop with app-defined states
    // while(true) {
	// 	switch(current_state) {
    //         case APP_STATE_SET_DEFAULT_BUFFER:
    //     }
    // }
}
