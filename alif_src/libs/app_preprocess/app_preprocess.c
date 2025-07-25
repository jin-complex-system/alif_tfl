#include "app_preprocess.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <precomputed_window/hann_window/hann_window_scale_2048.h>
#include <audio_dsp_fft.h>
#include <power_spectrum.h>
#include <mel_spectrogram.h>
#include <power_to_decibel.h>
#include <transpose_buffer.h>

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
    uint32_t output_buffer_length) {
#ifdef LOAD_AUDIO_AND_PREPROCESS
        
        assert(num_iterations > 0);
        assert(num_frames_to_read <= NUM_FRAMES);
        assert(CROPPED_NUM_FRAMES <= NUM_FRAMES);

        // const uint16_t total_iterations = num_iterations * NUM_SECONDS_DESIRED_AUDIO;
        const uint16_t total_iterations = num_iterations;

        uint8_t* shifted_buffer;
        uint32_t num_frames_process;
        uint32_t left_padding_length = 0;

        /// Will need to pad
        if (num_frames_to_read < CROPPED_NUM_FRAMES) {
            num_frames_process = num_frames_to_read;
    #ifdef LEFT_PADDING
            left_padding_length = (CROPPED_NUM_FRAMES - num_frames_process) / 2;
    #endif // LEFT_PADDING
            shifted_buffer = output_buffer + left_padding_length;
        }
        /// Will need to crop so process less frames
        else if (num_frames_to_read >= CROPPED_NUM_FRAMES) {
            num_frames_process = CROPPED_NUM_FRAMES;
            shifted_buffer = output_buffer;
        }
        assert(shifted_buffer != NULL);

        const uint32_t shifted_buffer_length = N_MELS * num_frames_process;
        const uint32_t right_padding_length = left_padding_length;

        /// Clear the buffers
        memset(power_spectrum_buffer, 0, sizeof(power_spectrum_buffer));
        memset(mel_spectrogram_buffer, 0, sizeof(mel_spectrogram_buffer));
        memset(output_buffer, 0, output_buffer_length);

        printf(
            "Preprocessing %u iterations with %lu frames and padded left with %lu, from original read frames %lu\r\n",
            total_iterations,
            num_frames_process,
            left_padding_length,
            num_frames_to_read);

        /// Compute mel spectrogram
        for (uint16_t iterator = 0; iterator < total_iterations; iterator++) {
            float max_mel = 1e-16f;

            for (uint32_t frame_iterator = 0; frame_iterator < num_frames_process; frame_iterator++) {
                const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;
                const uint32_t power_spectrum_iterator = 0;
                const uint32_t mel_iterator = frame_iterator * N_MELS;

                assert(audio_iterator < audio_input_buffer_length);

                compute_power_spectrum_audio_samples(
                    &audio_input_buffer[audio_iterator],
                    AUDIO_FRAME_LENGTH,
                    &power_spectrum_buffer[power_spectrum_iterator],
                    POWER_SPECTRUM_BUFFER_LENGTH,
                    NULL,
                    0u,
                    HANN_WINDOW_SCALE_2048_BUFFER,
                    HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
                );

                const float temp_max = compute_power_spectrum_into_mel_spectrogram(
                    &power_spectrum_buffer[power_spectrum_iterator],
                    POWER_SPECTRUM_LENGTH,
                    &mel_spectrogram_buffer[mel_iterator],
                    N_FFT,
                    SAMPLING_RATE_PER_SECOND,
                    MAX_FREQUENCY,
                    N_MELS
                );

                if (temp_max > max_mel) {
                    max_mel = temp_max;
                }
            }

            convert_power_to_decibel_and_scale(
                mel_spectrogram_buffer,
                shifted_buffer,
                shifted_buffer_length,
                max_mel);

            /// If needed, clear the left and right side of shifted_buffer
            #ifdef LEFT_PADDING
            if (right_padding_length > 0) {
                memset(
                    output_buffer,
                    0,
                    left_padding_length);
                memset(
                    shifted_buffer + shifted_buffer_length,
                    0,
                    right_padding_length);
            }
            #endif // LEFT_PADDING
        }

        /// Transpose output_buffer
        memset(mel_spectrogram_buffer, 0, sizeof(mel_spectrogram_buffer));
        uint8_t* temp_buffer = (uint8_t*)mel_spectrogram_buffer;

        memcpy(temp_buffer, output_buffer, sizeof(output_buffer));

        transpose_buffer(
            temp_buffer,
            output_buffer,
            NUM_FRAMES,
            N_MELS
        );

#endif // LOAD_AUDIO_AND_PREPROCESS
}
