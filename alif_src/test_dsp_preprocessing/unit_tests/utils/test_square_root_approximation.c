#include "test_square_root_approximation.h"

#include <square_root_approximation.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

const double FLOAT_ERROR_TOLERANCE = 1e-4;

void
test_square_root_approximation() {
    printf("Running test_square_root_approximation()");

    const float input_array[] = {
        0.0f, 1.0f, 4.0f, 12.0f, 127.0f,
    };
    const uint16_t
    input_array_length = sizeof(input_array) / sizeof(float);

    for (uint16_t iterator = 0; iterator < input_array_length; iterator++) {
        float current_input = input_array[iterator];

        const float expected_value = sqrtf(current_input);
        const float computed_value = square_root_approximation(current_input);

        const double float_difference = 
        abs((double)computed_value - (double)expected_value);

        if (float_difference > FLOAT_ERROR_TOLERANCE) {
            printf("Mismatch in test_square_root_approximation");
        }
    }

    printf("Done with test_square_root_approximation()");
}
