#ifndef INFERENCE_TF_H
#define INFERENCE_TF_H

#include <stdint.h>
#include "inference_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Setup inference using TensorFlow
 */
void
inference_tf_setup(void);

/**
 * Set input buffer using TensorFlow
 *
 * @param input_buffer
 * @param input_buffer_length
 */
void
inference_tf_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length);

/**
 * Get the output buffer using TensorFlow
 *
 * @param output_buffer
 * @param output_buffer_length
 */
void
inference_tf_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length);

/**
 * Perform prediction using TensorFlow
 */
void
inference_tf_predict();

#ifdef __cplusplus
}
#endif

#endif // INFERENCE_TF_H