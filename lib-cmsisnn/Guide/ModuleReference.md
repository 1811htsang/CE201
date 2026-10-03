# CMSIS-NN Module Reference

An overview of what lives in each directory. File counts refer to `.c` files in the directory. For parameters and
preconditions of any function, the Doxygen comment in `Include/arm_nnfunctions.h` is the authoritative source.

## Include/

| File | Purpose |
| --- | --- |
| `arm_nnfunctions.h` | Public integer API. Doxygen groups: `NNConv`, `FC`, `groupElementwise`, `Acti`, `Pooling`, `Softmax`, `Reshape`, `Transpose`, `Concatenation`, `SVDF`, `LSTM`, `Pad`. |
| `arm_nnfunctions_flt.h` | Public experimental float API. |
| `arm_nnsupportfunctions.h` | Support/helper API used by kernels (requantize, matrix multiply cores, data conversion). Also usable by integrators. |
| `arm_nnsupportfunctions_flt.h` | Float support API. |
| `arm_nn_types.h`, `arm_nn_types_flt.h` | Public parameter structs, status codes and enums. |
| `arm_nn_math_types.h`, `arm_nn_math_types_flt.h` | Feature-flag translation, limits macros, basic math types. |
| `arm_nn_tables.h` | Lookup tables (sigmoid/tanh, softmax). |
| `Internal/` | Not part of the public API: `arm_nn_config.h`, `arm_nn_compiler.h`, `arm_nn_activation_flt.h`, and shared fragments for float convolution, depthwise convolution, 1x1 convolution, min/max, transpose and concatenation. |

## Source/

### ConvolutionFunctions (48 files)

| Group | Files |
| --- | --- |
| Generic convolution | `arm_convolve_{s4,s8,s16,f16,f32}`, `arm_convolve_even_s4` |
| Shape-specialised | `arm_convolve_1x1_{s4,s8,f16,f32}`, `arm_convolve_1x1_{s4,s8}_fast`, `arm_convolve_1_x_n_{s4,s8,f16,f32}` |
| Wrappers | `arm_convolve_wrapper_{s4,s8,s16}` |
| Depthwise | `arm_depthwise_conv_{s4,s8,s16,f16,f32}`, `_s4_opt`, `_s8_opt`, `_3x3_s8`, `_fast_s16`, `arm_nn_depthwise_conv_s8_core`, wrappers `arm_depthwise_conv_wrapper_{s4,s8,s16}` |
| Transpose convolution | `arm_transpose_conv_{s8,f16,f32}`, `arm_transpose_conv_wrapper_s8` |
| Buffer sizes (files holding the `*_get_buffer_size[_dsp\|_mve]` functions) | `arm_convolve_get_buffer_sizes_{s4,s8,s16}`, `arm_depthwise_conv_get_buffer_sizes_{s4,s8,s16}`, `arm_transpose_conv_get_buffer_sizes_s8` |
| Matrix multiply kernels | `arm_nn_mat_mult_s8`, `arm_nn_mat_mult_kernel_{s8_s16,s4_s16,s16,row_offset_s8_s16}` |

### FullyConnectedFunctions (15)

`arm_fully_connected_{s4,s8,s16,f16,f32}`, `arm_fully_connected_per_channel_s8`, `arm_fully_connected_wrapper_s8`,
`arm_fully_connected_get_buffer_sizes_{s8,s16}`, `arm_batch_matmul_{s8,s16,f16,f32}`, `arm_vector_sum_s8`,
`arm_vector_sum_s8_s64`. The vector-sum helpers precompute the filter-sum term used to fold input offsets into the bias.

### PoolingFunctions (10)

`arm_avgpool_{s8,s16}`, `arm_max_pool_{s8,s16}`, `arm_avg_pool_{f16,f32}`, `arm_max_pool_{f16,f32}`,
`arm_avgpool_get_buffer_sizes_{s8,s16}`.

### SoftmaxFunctions (7)

`arm_softmax_{s8,s16,u8,s8_s16,f16,f32}` and the shared `arm_nn_softmax_common_s8`.

### ActivationFunctions (6)

`arm_relu_q7`, `arm_relu_q15`, `arm_relu6_s8`, `arm_nn_activation_{s16,f16,f32}` (sigmoid/tanh through
`arm_nn_activation_type`).

### BasicMathFunctions (17)

Elementwise `arm_elementwise_add_{s8,s16,f16,f32}`, `arm_elementwise_mul_{s8,s16,f16,f32}`,
`arm_elementwise_mul_acc_s16`, `arm_elementwise_mul_s16_{batch_offset,s8}`, and `arm_maximum_*` / `arm_minimum_*` for
s8, f16 and f32.

### LSTMFunctions (4) and SVDFunctions (5)

- LSTM: `arm_lstm_unidirectional_{s8,s16,f16,f32}`. The per-step and gate computations live in `NNSupportFunctions`
  (`arm_nn_lstm_step_*`, `arm_nn_lstm_calculate_gate_*`).
- SVDF: `arm_svdf_{s8,f16,f32}`, `arm_svdf_state_s16_s8`, `arm_svdf_get_buffer_sizes_s8`.

### Layout and shape operators

| Directory | Files |
| --- | --- |
| ConcatenationFunctions (6) | `arm_concatenation_s8_{w,x,y,z}`, `arm_concatenation_{f16,f32}` |
| PadFunctions (3) | `arm_pad_{s8,f16,f32}` |
| ReshapeFunctions (3) | `arm_reshape_{s8,f16,f32}` |
| TransposeFunctions (3) | `arm_transpose_{s8,f16,f32}` |

### NNSupportFunctions (58)

Low-level building blocks shared by the operator folders:

- **Matrix/vector multiply:** `arm_nn_mat_mult_nt_t_{s8,s8_s32,s16,s4,f16,f32}`, `arm_nn_mat_mul_core_{1x,4x}_s8`,
  `arm_nn_mat_mul_core_1x_s4`, `arm_nn_vec_mat_mult_t_{s8,s16,s16_s16,s4,per_ch_s8,svdf_s8}`,
  `arm_nn_vec_mat_mul_result_acc_*`, `arm_nn_mat_mult_nt_n_packed_*` (packed `NTxN` float weights).
- **Depthwise helpers:** `arm_nn_depthwise_conv_nt_t_{s8,padded_s8,s16,s4,f16,f32}`, `arm_nn_depthwise_conv3x3_*`,
  `arm_nn_depthwise_conv1d_k3_*`.
- **Float 1-D convolution and packing:** `arm_nn_conv1d_k{3,5}_*`, `arm_nn_pack_conv_patch_*`, `arm_nn_maxpool1d_*`.
- **Data conversion:** `arm_q7_to_q15_with_offset`, `arm_s8_to_s16_unordered_with_offset`.
- **Other:** `arm_batch_norm_{f16,f32}`, `arm_nn_transpose_conv_row_s8_s32`, `arm_get_buffer_size_*`,
  lookup tables in `arm_nntables` and `arm_nntables_flt`.

### Bindings

pybind11 C++ sources exposing host-side buffer-size getters: `arm_py_module.cpp` (module entry), `arm_py_backend.cpp`,
`arm_py_common.hpp`, and one file each for conv, depthwise conv, fully connected, average pool, SVDF and transpose conv.

## Operator and data type coverage

The README holds the authoritative table of operators against C / DSP / MVE and int8 / int16 / int4 / float. Notable
gaps: Minimum, Maximum, Pad and Transpose have no int16 or DSP-specific variants; SVDF and TransposeConv2D have no
int16; int4 is limited to Conv2D, DepthwiseConv2D and Fully Connected.
