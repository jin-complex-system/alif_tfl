#include "inference_tf.h"
#include "inference_definitions.h"

#include <tensorflow/lite/micro/kernels/micro_ops.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>

// Add an include to the recording API:
#include <tensorflow/lite/micro/recording_micro_interpreter.h>

#include <cassert>

#include <model_orbw_19_Q_HE_vela.h>

static const
tflite::Model*
s_model = nullptr;

static
tflite::MicroInterpreter*
s_interpreter = nullptr;

/// An area of memory to use for input, output, and intermediate arrays.
/// (Can be adjusted based on the model needs.)
constexpr uint32_t
kTensorArenaSize = 300u * 1000u;

#ifdef TENSORARENA_NONCACHE
static uint8_t classifier_tensorArena[kTensorArenaSize] __ALIGNED(16) __attribute__((section("NonCacheable")));
#else
static
uint8_t
s_tensorArena[kTensorArenaSize] __attribute__((aligned(16)));
#endif // TENSORARENA_NONCACHE

// static
// tflite::MicroMutableOpResolver<26> mutableAllOpResolver; // NOLINT

// static inline
// void
// add_operators(void) {
//     mutableAllOpResolver.AddAdd();
//     mutableAllOpResolver.AddBatchMatMul();
//     mutableAllOpResolver.AddConv2D();
//     mutableAllOpResolver.AddDepthwiseConv2D();
//     mutableAllOpResolver.AddFullyConnected();
//     mutableAllOpResolver.AddAveragePool2D();
//     mutableAllOpResolver.AddSoftmax();
//     mutableAllOpResolver.AddTranspose();
//     mutableAllOpResolver.AddTransposeConv();
//     mutableAllOpResolver.AddUnidirectionalSequenceLSTM();
//     mutableAllOpResolver.AddEthosU();
//     mutableAllOpResolver.AddQuantize();
//     mutableAllOpResolver.AddMaxPool2D();
//     mutableAllOpResolver.AddReshape();
//     mutableAllOpResolver.AddSquaredDifference();
//     mutableAllOpResolver.AddRsqrt();
//     mutableAllOpResolver.AddMul();
//     mutableAllOpResolver.AddSub();
//     mutableAllOpResolver.AddConcatenation();
//     mutableAllOpResolver.AddShape();
//     mutableAllOpResolver.AddGather();
//     mutableAllOpResolver.AddGatherNd();
//     mutableAllOpResolver.AddStridedSlice();
//     mutableAllOpResolver.AddReduceMax();
//     mutableAllOpResolver.AddPack();
//     mutableAllOpResolver.AddLogistic();
//     mutableAllOpResolver.AddMean();
// }

void
inference_tf_setup(void) {
    // s_model = tflite::GetModel(model_orbw_19_Q_HE_vela_tflite);
	// assert(s_model->version() == TFLITE_SCHEMA_VERSION);

    // add_operators();
    
    // /// Build a recording interpreter
    // /// RecordingMicro interpreter can help deterine the actual memory
    // /// needed for runtime
    // {
    //     static tflite::RecordingMicroInterpreter record_interpreter(
    //     		s_model,
    // 			s_microOpResolver,
    // 			s_tensorArena,
    // 			kTensorArenaSize,
    // 			nullptr);
    //     PRINTF("Recording state of model at runtime\r\n");
    //     if (record_interpreter.Invoke() != kTfLiteOk) {
    //       PRINTF("Something went wrong with invoking interpreter for recording\r\n");
    //     }
    //     /// Print out detailed allocation information:
    //     else {

    //     	const auto microallocator =
    //     			record_interpreter.GetMicroAllocator();

    //     	/// Actual buffer needed for inference
	// 		microallocator.PrintAllocations();
    //     	PRINTF(
    //     			"Actual buffer needed: %d bytes\r\n",
	// 				microallocator.GetSimpleMemoryAllocator()->GetUsedBytes());
    //     }
    // }
}

void
inference_tf_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length) {
    // TODO: Set input buffer
}

void
inference_tf_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length) {
    // TODO: Get output buffer
}

void
inference_tf_predict() {
    // TODO: Perform prediction

    // TfLiteStatus invoke_status = interpreter.Invoke();
}
