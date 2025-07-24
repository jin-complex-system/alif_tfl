#include "test_app_preprocess.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <sd_card.h>

#include <app_preprocess.h>
#include <parameters.h>

#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH] __attribute__((aligned(16)));

uint8_t
scale_mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

uint8_t
output_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

static inline
void
test_app_preprocess_setup(void) {
    sd_card_setup();
    setup_preprocess(N_FFT);
}

void
test_app_preprocess(void) {
    test_app_preprocess_setup();

    const char INPUT_DIRECTORY[] = "test_preprocess";
    const char INPUT_FILENAME[] = "us8k_24074-1-0-2-mel_uint.edge";
    const char OUTPUT_FILENAME[] = "us8k_24074-1-0-2-mel_uint.edge";

    bool success = 0;
    DIR read_directory;
    uint32_t length_read = 0;
    char* filepath = NULL;

    /// Read audio file
    filepath = (char*)(&power_spectrum_buffer[0]);
    sprintf(
        filepath,
        "%s/%s",
        INPUT_DIRECTORY,
        INPUT_FILENAME
    );
    success = 
    sd_card_open_directory(
        INPUT_DIRECTORY,
        sizeof(INPUT_DIRECTORY),
        &read_directory);
    assert(success == true);

    length_read = 0;
    length_read = sd_card_read_from_file(
            audio_input_buffer,
            sizeof(audio_input_buffer),
            filepath);
    printf("Input Length read: %u\r\n", length_read);

    const uint32_t num_valid_audio_elements = 
    length_read / sizeof(audio_data_type);
    uint32_t num_frames_to_read = 
    (num_valid_audio_elements / HOP_LENGTH - N_FFT / HOP_LENGTH) + 1;

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

    /// Read preprocessed data
    filepath = (char*)(&power_spectrum_buffer[0]);
    sprintf(
        filepath,
        "%s/%s",
        INPUT_DIRECTORY,
        OUTPUT_FILENAME
    );
    length_read = 0;
    length_read = sd_card_read_from_file(
            output_buffer,
            sizeof(output_buffer),
            filepath);
    printf("Output Length read: %u\r\n", length_read);

    const int memcmp_result = memcmp(
        output_buffer,
        scale_mel_spectrogram_buffer,
        sizeof(output_buffer));
    printf("memcmp result: %i\r\n", memcmp_result);
    

    printf("Finished testing app_preprocess\r\n");
}