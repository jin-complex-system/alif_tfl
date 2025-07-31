# alif_src

# Setup Project

See [ALIF_README.md](/ALIF_README.md) and [Getting Started](/doc_alif/getting_started.md) for the setup

## How to Program on Visual Studio Code

1. Determine your COM port of your device. The lower COM port is for programming while the upper COM port is for UART messages
    - On this example, `COM6` is for programming and `COM9` is `UART2` for Alif Ensemble Dev Kit HE core output
2. Open a UART program (like Putty) to the UART (`COM9`), with the settings at 115200 baud rate 
3. Navigate to CMSIS Extension
4. Click on the 'Buid Solution` icon (looks like a hammer)
5. Press `F1` on your keyboard and select `Tasks: Run Task`
6. Select `Program with Security Toolkit (select COM port)`
7. On the terminal, when the COM port list shows up, select the lowest COM port
    - Type out the entire name `COM6` rather than just `6`
8. When programming is finished, view the UART console output

# Preprocess Project

The main project that demonstrates preprocessing audio and conduct inference.

## Project Recreation

To re-create the project from scratch, see [`recreate_new_project_from_blinky.md`](docs/recreate_new_project_from_blinky.md)

## Instructions on How to Use Preprocess

1. If necessary, update the [models](/models/)
- Note that current builds only support the HE core at the moment
- If needing to quantise the model, use [quantise_using_eIQ_toolkit.md](/docs/quantise_using_eIQ_toolkit.md)
- Make sure the model array is `nn_model[]` like the following format:
```C
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <stdint.h>

static const
uint8_t nn_model[] __attribute__((aligned(16))) = {
}

static const
uint32_t
nn_model_len = sizeof(nn_model); // Adjust according to length

#endif // MODEL_DATA_H
```

- operators would need to be updated inside [inference_tf.cpp](/libs/inference/inference_tf.cpp):
```C
/// Number of operators
static
tflite::MicroMutableOpResolver<27> s_microOpResolver; // NOLINT

static inline
void
add_operators(void) {
    // s_microOpResolver.AddAdd();
}
```
2. Navigate to [`parameters.h`](/preprocess/parameters.h) and determine which paraemters need to be configure
    - Comment out `#define LOAD_AUDIO_AND_PREPROCESS           1` if you want to load preprocessed data directly into the model
    - Comment in `#define SAVE_RESULT_TO_SD_CARD           1` if you want to save the predicted results into the SD card
    - Adjust `NUM_PREPROCESS_ITERATIONS` for the number of preprocess iterations
    - Adjust `NUM_INFERENCE_ITERATIONS` for the number of inference iterations

3. Navigate to [`inference_parameters.h](/libs/inference/inference_definitions.h) and determine which parameters need to be configure:
    - Comment out `#define USE_NPU_MODEL               1` if you want to not use the NPU-version of the model
    - Comment out `#define USE_MODELS_DSP_PREPROCESSING    1` if you use librosa's preprocessing version
    - Comment out `#define USE_ORBIWISE                    1` if you to use Urbansound8K model
    - Adjust changes to reflect the new model(s) if relevant

4. Insert a SD card with the following directories created:
    - Audio
        - `ub_audio` - audio data (16-bit signed audio) for Urbansound8K
        - `out_ubA` - Output for Urbansound8K audio predictions
        - `ow_audio` - audio data (16-bit signed audio) for Orbiwise
        - `out_owA` - Output for Orbiwise audio predictions
    - Preprocessed data
        - `ub_pre` - for preprocessed audio data (8-bit unsigned mel spectrogram) for Urbansound8K
        - `ub_owP` - Output for Urbansound8K preprocessed predictions
        - `ow_pre` - for preprocessed audio data (8-bit unsigned mel spectrogram) for Orbiwise
        - `out_owP` - Output for Orbiwise preprocessed predictions

5. [Program the Alif Ensemble Dev Kit](#how-to-program-on-visual-studio-code)

6. Check the UART output

7. To measure timing, measure the following LED lights:
    - Green LED light toggles between preprocessing
    - Blue LED light toggles between inference
    
8. When it is done, grab the output files from the SD card

9. If necessary, conduct post-processing of using [Python scripts](../python_src/README.md)

# Model_inference Project

The project to demonstrate only model inference.

# test_dsp_preprocessing Project

Project containing unit tests for dsp_preprocessing. See [TODO](#todo)

# TODO

1. Investigate why preprocessing and inference yields the same results
    - Unlikely ARM-specific implementation is the root cause
    - FFT from dsp_preprocessing is different from desktop than running on edge device
2. Implement microphones on Alif Ensemble Dev Kit 2 into workflow
3. HP core on Preprocess does not work with the memory configuration used in HE core
4. Inference speed for the current memory configuration on HE core is slower than expected

# References
- [Alif ML Embedded Evaluation Kit](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/tree/main)
- [Alif VSCode Template](https://github.com/alifsemi/alif_vscode-template)
- [Deployment of TFLite Models on Alif Semiconductor’s MCUs](https://alifsemi.com/whitepaper/ai-ml-deployment-of-tflite-models/)
- [Arm Cortex-M55 Optimization and Tools](https://alifsemi.com/whitepaper/cortex-m55-optimization-and-tools/)
