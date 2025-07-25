#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

// #define LOAD_AUDIO_AND_PREPROCESS           1
// #define SAVE_RESULT_TO_SD_CARD              1 

/// Varying parameters
#define AUDIO_SAMPLE_DATA_BITS              16
#if AUDIO_SAMPLE_DATA_BITS == 16
typedef int16_t audio_data_type;

#elif AUDIO_SAMPLE_DATA_BITS == 32
typedef int32_t audio_data_type;
#endif // AUDIO_SAMPLE_DATA_BITS

#define N_FFT                               2048
#define HOP_LENGTH                          512

#define N_MELS                              64

#define NUM_SECONDS_DESIRED_AUDIO           7
#define NUM_SECONDS_AUDIO                   NUM_SECONDS_DESIRED_AUDIO
#define SAMPLING_RATE_PER_SECOND            44100

#define TOP_DECIBEL                         80.0f
#define MAX_FREQUENCY                       8000u

/// Power spectrum parameters
#define AUDIO_FRAME_LENGTH                  N_FFT
#define AUDIO_BUFFER_MINIMUM_LENGTH         (SAMPLING_RATE_PER_SECOND * NUM_SECONDS_AUDIO)
#define DSP_ACOUSTIC_INPUT_BUFFER_LENGTH    N_FFT

#define NUM_FRAMES                          ((AUDIO_BUFFER_MINIMUM_LENGTH / HOP_LENGTH - N_FFT/HOP_LENGTH) + 1)
#define REAL_FREQUENCY_BINS_LENGTH          ((N_FFT / 2) + 1)
#define POWER_SPECTRUM_LENGTH               REAL_FREQUENCY_BINS_LENGTH
#define POWER_SPECTRUM_BUFFER_LENGTH        (DSP_ACOUSTIC_INPUT_BUFFER_LENGTH * 2)

/// Buffer parameters
#define MEL_SPECTROGRAM_INPUT_SIZE          (AUDIO_FRAME_LENGTH * 2)
#define MEL_SPECTROGRAM_BUFFER_LENGTH       (N_MELS * NUM_FRAMES)

/// Misc
#define NUM_PREPROCESS_ITERATIONS           10
#define NUM_INFERENCE_ITERATIONS            10

#define CROPPED_NUM_FRAMES                  384
#define CROPPED_MEL_SPEC_BUFFER_LENGTH      (N_MELS * CROPPED_NUM_FRAMES)
#define LEFT_PADDING                        1


#endif // PARAMETERS_H