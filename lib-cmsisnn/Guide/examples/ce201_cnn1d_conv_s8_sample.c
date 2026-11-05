#include "ce201_cnn1d_conv_s8_sample.h"

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
    int32_t scratch_buffer_size)
{
    if (conv_params == 0 || quant_params == 0 || input_dims == 0 || input_data == 0 ||
        filter_dims == 0 || filter_data == 0 || bias_dims == 0 || bias_data == 0 ||
        output_dims == 0 || output_data == 0)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    /* A 1-D CNN is lowered to a height-1 2-D convolution. */
    if (input_dims->h != 1 || filter_dims->h != 1 || output_dims->h != 1 ||
        filter_dims->n != output_dims->c || filter_dims->c != input_dims->c ||
        bias_dims->c != output_dims->c)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    const int32_t required_scratch = arm_convolve_wrapper_s8_get_buffer_size(
        conv_params, input_dims, filter_dims, output_dims);
    if (required_scratch < 0 || required_scratch > scratch_buffer_size ||
        (required_scratch > 0 && scratch_buffer == 0))
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    cmsis_nn_context context = {
        .buf = scratch_buffer,
        .size = required_scratch,
    };

    return arm_convolve_wrapper_s8(
        &context,
        conv_params,
        quant_params,
        input_dims,
        input_data,
        filter_dims,
        filter_data,
        bias_dims,
        bias_data,
        output_dims,
        output_data);
}
