#ifndef APP_PREPROCESS_H
#define APP_PREPROCESS_H

#include <stdint.h>
#include <string.h>

#include <parameters.h>

/// dsp_preprocessing
#include <power_spectrum.h>

static
float
power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH] __attribute__((aligned(16)));

#ifdef LOAD_AUDIO_AND_PREPROCESS
static
float
mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));
#endif // LOAD_AUDIO_AND_PREPROCESS

/**
 * Setup preprocess
 * 
 * @param n_fft
 */
inline
void
setup_preprocess(
    const uint16_t n_fft) {
#ifdef LOAD_AUDIO_AND_PREPROCESS

        initialise_power_spectrum(n_fft);
        
        memset(power_spectrum_buffer, 0, sizeof(power_spectrum_buffer));
        memset(mel_spectrogram_buffer, 0, sizeof(mel_spectrogram_buffer));
#endif // LOAD_AUDIO_AND_PREPROCESS
}

/**
 * Preprocess
 * 
 * @param audio_input_buffer
 * @param audio_input_buffer_length
 * @param num_iterations
 * @param num_frames_to_read
 * @param scaled_buffer
 * @param scaled_buffer_length
 */
void
preprocess(
    const int16_t* audio_input_buffer,
    uint32_t audio_input_buffer_length,
    const uint16_t num_iterations,
    const uint32_t num_frames_to_read,
    uint8_t* scaled_buffer,
    uint32_t scaled_buffer_length);

#endif // APP_PREPROCESS_H
