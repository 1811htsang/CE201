#pragma once

#include <stdint.h>

#include "arm_nnfunctions.h"

/*
 * CNN-1D is represented as a height-1 CMSIS-NN convolution:
 * input  [N, 1, W, C_IN]
 * filter [C_OUT, 1, K, C_IN]
 * output [N, 1, W_OUT, C_OUT]
 *
 * The dimensions and quantization values must come from the converted model.
 */
arm_cmsis_nn_status ce201_cnn1d_conv_s8(
    const cmsis_nn_conv_params *conv_params,
    const cmsis_nn_per_channel_quant_params *quant_params,
    const cmsis_nn_dims *input_dims,
    const int8_t *input_data,
    const cmsis_nn_dims *filter_dims,
    const int8_t *filter_data,
    const cmsis_nn_dims *bias_dims,
    const int32_t *bias_data,
    const cmsis_nn_dims *output_dims,
    int8_t *output_data,
    void *scratch_buffer,
    int32_t scratch_buffer_size);
