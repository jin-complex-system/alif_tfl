#ifndef APP_PREPROCESS_H
#define APP_PREPROCESS_H

#include <stdint.h>
#include <string.h>

#include <parameters.h>

/// dsp_preprocessing
#include <power_spectrum.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
//DOM-IGNORE-END

#ifdef LOAD_AUDIO_AND_PREPROCESS

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
#endif // LOAD_AUDIO_AND_PREPROCESS
}

/**
 * 
 * @param audio_input_buffer
 * @param audio_input_buffer_length 
 * @param power_spectrum_buffer 
 * @param power_spectrum_buffer_length 
 * @param mel_spectrogram_buffer 
 * @param mel_spectrogram_buffer_length 
 * @param num_iterations 
 * @param num_frames_to_read 
 * @param output_buffer 
 * @param output_buffer_length 
 */
void
preprocess(
    const int16_t* audio_input_buffer,
    uint32_t audio_input_buffer_length,
    float* power_spectrum_buffer,
    const uint32_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
    const uint32_t mel_spectrogram_buffer_length,
    const uint16_t num_iterations,
    const uint32_t num_frames_to_read,
    uint8_t* output_buffer,
    uint32_t output_buffer_length);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
};
#endif
//DOM-IGNORE-END

#endif // APP_PREPROCESS_H
