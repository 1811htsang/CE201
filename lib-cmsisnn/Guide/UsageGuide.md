# Using CMSIS-NN

How to add CMSIS-NN to an application and call its C API, plus the optional Python helpers. Function signatures and
parameter ranges are quoted from `Include/arm_nnfunctions.h`; check the Doxygen comment there before relying on a
detail, because ranges and preconditions differ between functions.

## 1. Integration options

| Route | When to use |
| --- | --- |
| Through a framework | LiteRT/TFLite Micro or the ExecuTorch Arm Cortex-M backend call CMSIS-NN for you. You only enable the CMSIS-NN kernels in that framework. |
| Static library | Build `cmsis-nn` with CMake (see [BuildAndTest.md](./BuildAndTest.md)) and link it. |
| Source or CMSIS-Pack | Add `Source/**/*.c` and the `Include/` directory to your project, or use the CMSIS-Pack. |

Compile with the real target flags (for example `-mcpu=cortex-m55`) so that the feature macros `ARM_MATH_DSP` /
`ARM_MATH_MVEI` are derived and the optimised kernels are selected (see [Architecture.md](./Architecture.md)).

CMSIS-NN can be built without CMSIS-Core. To add CMSIS-Core headers to the
library target, configure with
`-DCMSISNN_USE_CMSIS_CORE=ON -DCMSISNN_CMSIS_CORE_PATH=<path to CMSIS>`.
The path may point to a CMSIS root or directly to its `Include` directory.

## 2. Headers

```c
#include "arm_nnfunctions.h"          // operators + *_get_buffer_size
#include "arm_nnsupportfunctions.h"   // optional: helpers such as arm_vector_sum_s8
```

For the experimental float API, define `ARM_NN_ENABLE_F32` and/or `ARM_NN_ENABLE_F16` (CMake options of the same
name), and include `arm_nnfunctions_flt.h`.

## 3. The calling pattern

Every context-based kernel follows the same steps:

1. **Describe tensors** with `cmsis_nn_dims` (`n`, `h`, `w`, `c`). Layout is NHWC. Filters are `[C_OUT, HK, WK, C_IN]`.
2. **Fill the layer parameters** (`cmsis_nn_conv_params`, `cmsis_nn_fc_params`, `cmsis_nn_pool_params`, ...).
3. **Provide requantization values**: multiplier and shift, per output channel
   (`cmsis_nn_per_channel_quant_params`) or per tensor (`cmsis_nn_per_tensor_quant_params`).
4. **Ask for scratch size** with the matching `*_get_buffer_size()` and allocate it. A size of 0 means no buffer is
   needed. Pass it in `cmsis_nn_context`.
5. **Call the kernel** and check the returned `arm_cmsis_nn_status`.

Kernels never allocate memory themselves. The documented contract also says the caller should clear the scratch
buffer if that matters for security.

### Quantization inputs

Values come from the quantized model, using the TensorFlow Lite int8 specification:

- `input_offset` and `output_offset` are derived from the tensor zero points. For TFLM integration, `input_offset` is
  the negative of the input zero point and `output_offset` is the output zero point. The struct comments in
  `arm_nn_types.h` word this differently, so rely on the ranges in each function's Doxygen: for the s8 convolution
  `input_offset` is in [-127, 128] and `output_offset` in [-128, 127].
- `multiplier` and `shift` are the fixed-point form of the real requantization scale
  (`input_scale * filter_scale / output_scale`), produced the same way TFLite does.
- `activation.min` / `activation.max` clamp the int8 result. Use -128 / 127 for no activation, or tighter values to
  fuse ReLU or ReLU6.

## 4. Example: int8 convolution

```c
#include <stdlib.h>
#include "arm_nnfunctions.h"

arm_cmsis_nn_status run_conv(const int8_t *input, const int8_t *filter, const int32_t *bias,
                             int32_t *mult, int32_t *shift, int8_t *output)
{
    cmsis_nn_dims input_dims  = {.n = 1, .h = 8, .w = 8, .c = 16};
    cmsis_nn_dims filter_dims = {.n = 8, .h = 3, .w = 3, .c = 16};   // n = C_OUT
    cmsis_nn_dims bias_dims   = {.n = 1, .h = 1, .w = 1, .c = 8};    // c = C_OUT
    cmsis_nn_dims output_dims = {.n = 1, .h = 6, .w = 6, .c = 8};

    cmsis_nn_conv_params conv_params = {
        .input_offset  = 0,                      // from the model's input zero point
        .output_offset = 0,                      // from the model's output zero point
        .stride        = {.w = 1, .h = 1},
        .padding       = {.w = 0, .h = 0},
        .dilation      = {.w = 1, .h = 1},
        .activation    = {.min = -128, .max = 127},
    };

    cmsis_nn_per_channel_quant_params quant_params = {.multiplier = mult, .shift = shift};  // C_OUT entries each

    cmsis_nn_context ctx = {.buf = NULL, .size = 0};
    ctx.size = arm_convolve_wrapper_s8_get_buffer_size(&conv_params, &input_dims, &filter_dims, &output_dims);
    if (ctx.size > 0)
    {
        ctx.buf = malloc(ctx.size);          // on an MCU, use a static or arena buffer instead
        if (ctx.buf == NULL) { return ARM_CMSIS_NN_FAILURE; }
    }

    arm_cmsis_nn_status status = arm_convolve_wrapper_s8(&ctx, &conv_params, &quant_params,
                                                         &input_dims, input,
                                                         &filter_dims, filter,
                                                         &bias_dims, bias,
                                                         &output_dims, output);
    free(ctx.buf);
    return status;
}
```

`arm_convolve_wrapper_s8` picks the best of `arm_convolve_1x1_s8_fast`, `arm_convolve_1x1_s8`, `arm_convolve_1_x_n_s8`
or `arm_convolve_s8` from the shapes. The output dimensions must already be consistent with input, filter, stride and
padding; the library does not compute them for you. The same pattern, with `cmsis_nn_dw_conv_params` (adds `ch_mult`),
applies to `arm_depthwise_conv_wrapper_s8`.

## 5. Other common operators

All signatures below are from `arm_nnfunctions.h`.

**Fully connected (s8)**: per-tensor quantization, filter dims `[N, C]` where `N` is the accumulation depth
(`H*W*C_IN` of the input) and `C` is `C_OUT`. `fc_params.filter_offset` must be 0.

```c
arm_cmsis_nn_status arm_fully_connected_s8(const cmsis_nn_context *ctx, const cmsis_nn_fc_params *fc_params,
                                           const cmsis_nn_per_tensor_quant_params *quant_params,
                                           const cmsis_nn_dims *input_dims, const int8_t *input_data,
                                           const cmsis_nn_dims *filter_dims, const int8_t *filter_data,
                                           const cmsis_nn_dims *bias_dims, const int32_t *bias_data,
                                           const cmsis_nn_dims *output_dims, int8_t *output_data);
// scratch: arm_fully_connected_s8_get_buffer_size(&filter_dims)
```

**Pooling (s8)**: `arm_avgpool_s8` and `arm_max_pool_s8` take `cmsis_nn_pool_params` (stride, padding, activation).
`filter_dims` carries the window in `h` and `w`. Average pooling needs scratch from
`arm_avgpool_s8_get_buffer_size(dim_dst_width, ch_src)`; the Doxygen documents input dims as `[H, W, C_IN]`.

**Softmax (s8)**: no context and no status return.

```c
void arm_softmax_s8(const int8_t *input, const int32_t num_rows, const int32_t row_size,
                    const int32_t mult, const int32_t shift, const int32_t diff_min, int8_t *output);
```

**Elementwise add (s8)**: takes the offsets, multipliers and shifts of both inputs and the output as individual
arguments (`arm_elementwise_add_s8`), and returns a status. **ReLU6**: `void arm_relu6_s8(int8_t *data, uint16_t size)`
works in place.

**Return codes**: `ARM_CMSIS_NN_SUCCESS`, `ARM_CMSIS_NN_ARG_ERROR` (argument constraint failed),
`ARM_CMSIS_NN_NO_IMPL_ERROR`, `ARM_CMSIS_NN_FAILURE`. Functions declared `void` do not report errors.

## 6. Choosing between a wrapper and a specific kernel

- Use the `*_wrapper_*` function when shapes are only known at run time. It costs a few branches per call.
- Call a specific kernel (for example `arm_convolve_1x1_s8_fast`) when you know the shapes at build time. These have
  extra preconditions listed in their Doxygen comments, and their own `*_get_buffer_size`.
- The scratch size must be queried for the same function you call. The wrapper's size getter covers whichever kernel
  the wrapper may choose.
- Setting `NN_DISABLE_SPECIALIZATION` forces the generic paths, which is useful to cross-check a specialised kernel.

## 7. Sizing buffers on a host (Python)

An optional pybind11 module, `cmsis_nn`, exposes the buffer-size getters so a model converter or planner running on a
PC can compute scratch sizes without an Arm toolchain. It does not run inference.

Build and install:

```bash
cmake -S . -B build -DCMSISNN_BUILD_PYBIND=ON
cmake --build build
pip wheel . -w dist && pip install dist/cmsis_nn-*.whl
```

Enums: `Backend` (`MVE`, `DSP`, `SCALAR`), `DataType` (`A8W4`, `A8W8`, `A16W8`) and `CortexM` (`M0`, `M0PLUS`, `M3`,
`M4`, `M7`, `M23`, `M33`, `M35P`, `M55`, `M85`). `resolve_backend(core)` maps a core to its backend: M55 and M85 to
`MVE`; M4, M7, M33 and M35P to `DSP`; M0, M0PLUS, M3 and M23 to `SCALAR`.

| Function | Arguments after `backend, data_type` |
| --- | --- |
| `convolve_wrapper_buffer_size` | `input_nhwc, filter_nhwc, output_nhwc, padding_hw, stride_hw, dilation_hw`, optional `input_offset=0, output_offset=0, activation_min=-128, activation_max=127` |
| `depthwise_conv_wrapper_buffer_size` | same as convolution plus required `ch_mult` after `dilation_hw` |
| `fully_connected_buffer_size` | `filter_nhwc` |
| `svdf_buffer_size` | `filter_nhwc` |
| `avgpool_buffer_size` | `dim_dst_width, ch_src` |
| `transpose_conv_buffer_size` | `input_nhwc, filter_nhwc, output_nhwc, padding_hw, stride_hw, dilation_hw`, optional `padding_offsets_hw, input_offset, output_offset, activation_min, activation_max` |
| `transpose_conv_reverse_conv_buffer_size` | `input_nhwc, filter_nhwc, padding_hw, stride_hw`, optional `dilation_hw=(1, 1)`, `padding_offsets_hw` and the offset/activation arguments |

```python
import cmsis_nn

backend = cmsis_nn.resolve_backend(cmsis_nn.CortexM.M55)
size = cmsis_nn.convolve_wrapper_buffer_size(
    backend, cmsis_nn.DataType.A8W8,
    input_nhwc=[1, 8, 8, 16], filter_nhwc=[8, 3, 3, 16], output_nhwc=[1, 6, 6, 8],
    padding_hw=[0, 0], stride_hw=[1, 1], dilation_hw=[1, 1],
)
print(size, "bytes")
```

Not every backend/data-type pair exists for every operator; an unsupported pair raises `ValueError`. The `_dsp` and
`_mve` C getters that these functions call are the ones documented as intended for host compilation.

## 8. Where to find working code

- `Tests/UnitTest/TestCases/test_arm_*/` has a complete call sequence for each function, including scratch-buffer
  handling, for example `test_arm_convolve_s8/test_arm_convolve_s8.c`.
- `Tests/Bindings/` shows the Python calls and compares them against the raw C getters.
- `Examples/README.md` points to external end-to-end examples (TFLite Micro micro_speech and the Corstone-300 FVP
  convolution example).
