#include "inference_tf.h"
#include "inference_definitions.h"

#ifdef USE_TENSORFLOW
#include <tensorflow/lite/micro/kernels/micro_ops.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>

// Add an include to the recording API:
#include <tensorflow/lite/micro/recording_micro_interpreter.h>
#endif // USE_TENSORFLOW

#include <cassert>
#include <stdio.h>

#ifdef USE_TENSORFLOW

#ifdef USE_NPU_MODEL
#include <model_orbw_19_Q_HE_vela.h>
#else
#include <model_orbw_19_Q_HE.h>
#endif // USE_NPU_MODEL

static const
tflite::Model*
s_model = nullptr;

static
tflite::MicroInterpreter*
s_interpreter = nullptr;

/// An area of memory to use for input, output, and intermediate arrays.
/// (Can be adjusted based on the model needs.)
constexpr uint32_t
kTensorArenaSize = 800u * 1024u;

#ifdef TENSORARENA_NONCACHE
static uint8_t
s_tensorArena[kTensorArenaSize] __ALIGNED(16) __attribute__((section("NonCacheable")));
#else
static uint8_t
s_tensorArena[kTensorArenaSize] __attribute__((aligned(16)));
#endif // TENSORARENA_NONCACHE

static
tflite::MicroMutableOpResolver<27> s_microOpResolver; // NOLINT

static inline
void
add_operators(void) {
    s_microOpResolver.AddAdd();
    s_microOpResolver.AddBatchMatMul();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddDepthwiseConv2D();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddAveragePool2D();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddTranspose();
    s_microOpResolver.AddTransposeConv();
    s_microOpResolver.AddUnidirectionalSequenceLSTM();

#ifdef USE_NPU_MODEL
	s_microOpResolver.AddEthosU();
#endif // USE_NPU_MODEL

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddSquaredDifference();
    s_microOpResolver.AddRsqrt();
    s_microOpResolver.AddMul();
    s_microOpResolver.AddSub();
    s_microOpResolver.AddConcatenation();
    s_microOpResolver.AddShape();
    s_microOpResolver.AddGather();
    s_microOpResolver.AddGatherNd();
    s_microOpResolver.AddStridedSlice();
    s_microOpResolver.AddReduceMax();
    s_microOpResolver.AddPack();
    s_microOpResolver.AddLogistic();
    s_microOpResolver.AddMean();
}
#endif // USE_TENSORFLOW

void
inference_tf_setup(void) {
#ifdef USE_TENSORFLOW

#ifdef USE_NPU_MODEL
	printf("Using NPU\r\n");
#else
	printf("No NPU\r\n");	
#endif // USE_NPU_MODEL

    s_model = tflite::GetModel(nn_model);
	printf("model version: %u\r\n", s_model->version());
	assert(s_model->version() == TFLITE_SCHEMA_VERSION);

    add_operators();

    /// Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
		s_model,
		s_microOpResolver,
		s_tensorArena,
        kTensorArenaSize);
    s_interpreter = &static_interpreter;
	printf("Added static interpreter\r\n");

    /// Allocate memory from the tensor_arena for the model's tensors
    const TfLiteStatus allocate_status =
    		s_interpreter->AllocateTensors();
	printf("Allocated tensors interpreter, with status: %i\r\n", allocate_status);
    assert(allocate_status == kTfLiteOk);

#endif // USE_TENSORFLOW
}

void
inference_tf_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length) {
	/// Check parameters
	{
		assert(input_buffer != nullptr);
		assert(input_buffer_length > 0);
	}
#ifdef USE_TENSORFLOW

	assert(s_interpreter != nullptr);
	TfLiteTensor* inputTensor = s_interpreter->input(0);
	assert(inputTensor != nullptr);
	assert(inputTensor->type == EXPECTED_INPUT_DATA_TYPE);

    #ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < inputTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * inputTensor->dims->data[dims_iterator];
	}
	printf("Input size: %u\r\n", tensor_size);
	// assert(input_buffer_length == tensor_size);
#endif // NDEBUG

	inference_input_data_type* tensor_input;
	if constexpr(EXPECTED_INPUT_DATA_TYPE == kTfLiteUInt8) {
		tensor_input = (inference_input_data_type *)inputTensor->data.uint8;
	}
	else if constexpr(EXPECTED_INPUT_DATA_TYPE == kTfLiteFloat32) {
		tensor_input = (inference_input_data_type *)inputTensor->data.f;
	}
	else if constexpr(EXPECTED_INPUT_DATA_TYPE == kTfLiteInt8) {
		tensor_input = (inference_input_data_type *)inputTensor->data.int8;
	}
	assert(tensor_input != nullptr);

	/// Load input
    {
        for (uint32_t tensor_iterator = 0; tensor_iterator < input_buffer_length; tensor_iterator++) {
        	tensor_input[tensor_iterator] =
        			input_buffer[tensor_iterator];
        }
    }
#endif // USE_TENSORFLOW
}

void
inference_tf_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length) {
    	/// Check parameters
	{
		assert(output_buffer != nullptr);
		assert(output_buffer_length > 0);
	}
#ifdef USE_TENSORFLOW
	assert(s_interpreter != nullptr);

	TfLiteTensor* outputtTensor;
	outputtTensor = s_interpreter->output(1);

    assert(outputtTensor != nullptr);
    assert(outputtTensor->type == EXPECTED_OUTPUT_DATA_TYPE);

#ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < outputtTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * outputtTensor->dims->data[dims_iterator];
	}
	printf("Output size: %u\r\n", tensor_size);
	// assert(output_buffer_length == tensor_size);
	
#endif // NDEBUG

	inference_output_data_type* tensor_output;
	if constexpr(EXPECTED_OUTPUT_DATA_TYPE == kTfLiteUInt8) {
		tensor_output = (inference_output_data_type *)outputtTensor->data.uint8;
	}
	else if constexpr(EXPECTED_OUTPUT_DATA_TYPE == kTfLiteFloat32) {
		tensor_output = (inference_output_data_type *)outputtTensor->data.f;
	}
	else if constexpr(EXPECTED_OUTPUT_DATA_TYPE == kTfLiteInt8) {
		tensor_output = (inference_input_data_type *)outputtTensor->data.int8;
	}
	assert(tensor_output != nullptr);

    /// Unload output buffer
    {
        for (uint32_t tensor_iterator = 0; tensor_iterator < output_buffer_length; tensor_iterator++) {
        	inference_output_data_type my_output = tensor_output[tensor_iterator];
        	output_buffer[tensor_iterator] = my_output;
        }
    }
#endif // USE_TENSORFLOW
}

void
inference_tf_predict(void) {
#ifdef USE_TENSORFLOW

	printf("Invoke interpreter\r\n");
	const auto tflite_status =
			s_interpreter->Invoke();
	printf("Inference status: %u\r\n", tflite_status);
	// TODO: Investigate intrepreter result and why NPU does not run properly
	
	assert(tflite_status == kTfLiteOk);
#endif // USE_TENSORFLOW
}
