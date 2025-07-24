#include "test_app_preprocess.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <sd_card.h>

#include <app_preprocess.h>
#include <parameters.h>

/// Constants
#include <audio/us8k_24074_1_0_2.h>
#include <audio/us8k_24074_1_0_2_power_spectrum.h>
#include <audio/us8k_24074_1_0_2_mel_spec.h>
#include <audio/us8k_24074_1_0_2_scaled_mel_spec.h>

#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH] __attribute__((aligned(16)));

uint8_t
scale_mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

uint8_t
output_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

/**
 * Print buffer
 * 
 * @param buffer
 * @param num_elements
 */
static inline
void
print_uint8_buffer(
    const uint8_t* buffer,
    const uint32_t num_elements) {
        assert(buffer != NULL);

        const uint8_t enter_new_line_counter = 32;

        uint8_t counter = 0;

        for (uint32_t iterator = 0; iterator < num_elements; iterator++) {
            printf("%u ,", (uint16_t)buffer[iterator]);

            counter++;

            if (counter == enter_new_line_counter) {
                printf("\r\n");
                counter = 0;
            }
            assert(counter < enter_new_line_counter);
        }
        printf("\r\n");
}

static inline
void
test_app_preprocess_setup(void) {
    sd_card_setup();
    setup_preprocess(N_FFT);
}

static inline
void
test_from_sd_card(void) {
    const char INPUT_DIRECTORY[] = "test_preprocess";
    const char INPUT_FILENAME[] = "us8k_24074-1-0-2-mel_uint.edge";
    const char OUTPUT_FILENAME[] = "us8k_24074-1-0-2-mel_uint.edge";

    bool success = 0;
    DIR read_directory;
    uint32_t length_read = 0;
    char* filepath = NULL;

    /// Clear buffers
    memset(audio_input_buffer, 0, sizeof(audio_input_buffer));
    memset(scale_mel_spectrogram_buffer, 0, sizeof(scale_mel_spectrogram_buffer));
    memset(output_buffer, 0, sizeof(output_buffer));

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
    printf("test_from_sd_card - Input Length read: %u\r\n", length_read);

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
    printf("test_from_sd_card - Output Length read: %u\r\n", length_read);

    const int memcmp_result = memcmp(
        output_buffer,
        scale_mel_spectrogram_buffer,
        sizeof(output_buffer));
    printf("test_from_sd_card - memcmp result: %i\r\n", memcmp_result);

    printf("test_from_sd_card - Done\r\n");
}

static inline
void
test_from_saved_headers(void) {
    /// Clear buffers
    memset(audio_input_buffer, 0, sizeof(audio_input_buffer));
    memset(scale_mel_spectrogram_buffer, 0, sizeof(scale_mel_spectrogram_buffer));

    memcpy(audio_input_buffer, US8K_24074_1_0_2_BUFFER, sizeof(US8K_24074_1_0_2_BUFFER));

    const uint32_t num_valid_audio_elements = US8K_24074_1_0_2_BUFFER_LENGTH;
    uint32_t num_frames_to_read = 
    (num_valid_audio_elements / HOP_LENGTH - N_FFT / HOP_LENGTH) + 1;

    /// Upper limit of number of frames
    if (num_frames_to_read > NUM_FRAMES) {
        num_frames_to_read = NUM_FRAMES;
    }

    uint32_t num_frames_comparison = 2;
    int memcmp_result;
    
    preprocess(
        audio_input_buffer,
        INPUT_BUFFER_LENGTH,
        NUM_PREPROCESS_ITERATIONS,
        num_frames_to_read,
        scale_mel_spectrogram_buffer,
        MEL_SPECTROGRAM_BUFFER_LENGTH
    );

    memcmp_result = 
    memcmp(scale_mel_spectrogram_buffer, US8K_24074_1_0_2_SCALED_MEL_SPEC_BUFFER, num_frames_comparison * N_MELS);
    printf("test_from_saved_headers - scale_mel_buffer memcmp result: %i\r\n", memcmp_result);
    
    printf("\r\nScale_mel_buffer: \r\n");
    if (memcmp_result != 0) {
        print_uint8_buffer(
            scale_mel_spectrogram_buffer,
            num_frames_comparison * N_MELS
        );

        printf("\r\nCompared: \r\n");
        print_uint8_buffer(
            US8K_24074_1_0_2_SCALED_MEL_SPEC_BUFFER,
            num_frames_comparison * N_MELS
        );
    }

    // memcmp_result = 
    // memcmp(power_spectrum_buffer, US8K_24074_1_0_2_SCALED_MEL_SPEC_BUFFER, num_frame_comparison * POWER_SPECTRUM_LENGTH);
    // printf("test_from_saved_headers - scale_mel_buffer memcmp result: %i\r\n", memcmp_result);

    printf("test_from_saved_headers - Done\r\n");
}

void
test_app_preprocess(void) {
    test_app_preprocess_setup();

    test_from_sd_card();
    test_from_saved_headers();

    printf("Finished testing app_preprocess\r\n");
}
