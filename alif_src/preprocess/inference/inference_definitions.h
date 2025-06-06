#ifndef INFERENCE_DEFINITIONS_H_
#define INFERENCE_DEFINITIONS_H_

#include <stdint.h>

#define USE_TENSORFLOW              1
#ifdef USE_TENSORFLOW
#include <tensorflow/lite/c/common.h>
#endif // USE_TENSORFLOW

typedef int8_t inference_input_data_type;
typedef int8_t inference_output_data_type;

#define EXPECTED_INPUT_DATA_TYPE    kTfLiteInt8
#define EXPECTED_OUTPUT_DATA_TYPE   kTfLiteInt8
#define NUM_CLASSES                 19

#endif /* INFERENCE_DEFINITIONS_H_ */