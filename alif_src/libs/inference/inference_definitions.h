#ifndef INFERENCE_DEFINITIONS_H_
#define INFERENCE_DEFINITIONS_H_

#include <stdint.h>

/// Comment in and out parameters as needed

#define USE_TENSORFLOW                  1
#define USE_NPU_MODEL                   1
// #define USE_MODELS_DSP_PREPROCESSING    1

// #define USE_ORBIWISE                    1

#ifdef USE_TENSORFLOW
#include <tensorflow/lite/c/common.h>
#endif // USE_TENSORFLOW

typedef uint8_t inference_input_data_type;
typedef int8_t inference_output_data_type;

#ifdef USE_TENSORFLOW
#define EXPECTED_INPUT_DATA_TYPE    kTfLiteUInt8
#define EXPECTED_OUTPUT_DATA_TYPE   kTfLiteInt8

#endif // USE_TENSORFLOW
#endif /* INFERENCE_DEFINITIONS_H_ */