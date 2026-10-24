# CMSIS-NN Architecture

## 1. Design principles

- **One function per file.** The file name matches the function name (`arm_convolve_s8.c` defines `arm_convolve_s8`).
  Each function is attached to a Doxygen group (for example `@addtogroup NNConv`).
- **Bit-exact with TensorFlow Lite (Micro).** Integer kernels follow the int8 / int16 quantization specification. Where
  TFL and TFLM reference kernels differ, CMSIS-NN follows TFLM.
- **Three implementations per operator, chosen at compile time.** Pure C, DSP extension (SIMD) and Helium/MVE. The
  user does not select one explicitly; compiler feature flags do.
- **Caller-owned memory.** Kernels do not allocate. Scratch memory is passed in through `cmsis_nn_context`, and its size
  is obtained from a matching `*_get_buffer_size` function.
- **Status codes.** Most kernels return `arm_cmsis_nn_status`
  (`ARM_CMSIS_NN_SUCCESS`, `ARM_CMSIS_NN_ARG_ERROR`, `ARM_CMSIS_NN_NO_IMPL_ERROR`, `ARM_CMSIS_NN_FAILURE`).

## 2. Naming conventions

Function names follow `arm_<operator>[_<variant>]_<type>`, where the type suffix encodes the data type:

| Suffix | Meaning |
| --- | --- |
| `_s4` | int4 weights with int8 activations |
| `_s8` | int8 |
| `_s16` | int16 activations (16x8 for most operators) |
| `_u8` | uint8 (softmax only) |
| `_f16`, `_f32` | experimental float16 / float32 |
| `_q7`, `_q15` | legacy ReLU variants |

Common variant markers:

- `_wrapper_` selects the best specialised kernel for the given shapes (see section 4).
- `_fast`, `_opt` are shape-constrained optimised kernels with extra preconditions.
- `_1x1`, `_1_x_n`, `_3x3` are shape-specialised kernels.
- `_get_buffer_size[_dsp|_mve]` and `_get_buffer_sizes_*` compute scratch requirements.

## 3. Architecture selection

`Include/arm_nn_math_types.h` translates compiler feature flags into CMSIS-NN macros (the same names CMSIS-DSP uses):

| Macro | Derived from | Meaning |
| --- | --- | --- |
| `ARM_MATH_DSP` | `__ARM_FEATURE_DSP == 1` | DSP extension, e.g. Cortex-M4, M7, M33 with DSP |
| `ARM_MATH_MVEI` | `__ARM_FEATURE_MVE & 1` | Helium integer, e.g. Cortex-M55, M85 |
| `ARM_MATH_MVEF` | `__ARM_FEATURE_MVE & 2` | Helium with floating point |
| `ARM_MATH_MVE_FLOAT16` | `__ARM_FEATURE_MVE & 2` | Helium with float16 |
| `ARM_MATH_AUTOVECTORIZE` | set by the user | Let the compiler auto-vectorise code that otherwise uses inline assembly |

If none of these are defined, the pure C path is compiled. `ARM_MATH_HELIUM` is also accepted and enables the MVE
macros. `Include/Internal/arm_nn_compiler.h` abstracts compiler differences (Arm Compiler 6, GCC, IAR) such as
`__STATIC_FORCEINLINE`, `__RESTRICT` and `__ASM`.

## 4. Wrapper dispatch

Wrappers pick a concrete kernel from the runtime tensor shapes and parameters. For example
`arm_convolve_wrapper_s8` ([source](../../Source/ConvolutionFunctions/arm_convolve_wrapper_s8.c)):

```
arm_convolve_wrapper_s8
 ├─ arm_nn_is_convolve_1x1()      → arm_nn_is_convolve_1x1_fast() ? arm_convolve_1x1_s8_fast : arm_convolve_1x1_s8
 ├─ arm_nn_is_convolve_1_x_n()    → arm_convolve_1_x_n_s8
 └─ otherwise                     → arm_convolve_s8
```

Wrappers exist for convolution (s4, s8, s16), depthwise convolution (s4, s8, s16), transpose convolution (s8) and fully
connected (s8). Applications that know their shapes at build time can call a specialised kernel directly and skip the
dispatch.

## 5. Scratch buffers

For each operator with scratch requirements there is a size getter declared in `Include/arm_nnfunctions.h`:

- Generic getter, e.g. `arm_convolve_wrapper_s8_get_buffer_size(...)`.
- Architecture-specific getters with `_dsp` or `_mve` suffix, e.g. `arm_convolve_wrapper_s8_get_buffer_size_mve(...)`.
  These allow sizing for a specific backend, including on a host.
- The getters are implemented in files named `arm_<op>_get_buffer_sizes_<type>.c` (for example
  `Source/ConvolutionFunctions/arm_convolve_get_buffer_sizes_s8.c`), which group all size getters for one data type.
  These files also contain private `__STATIC_INLINE` helpers that the public getters call.

The Python bindings (`Source/Bindings`) expose these host-side size getters; see [BuildAndTest.md](./BuildAndTest.md).

## 6. Public types

`Include/arm_nn_types.h` defines the shared parameter structs (all prefixed `cmsis_nn_`). Examples:

- `cmsis_nn_context` – scratch `buf` and `size`.
- `cmsis_nn_dims` – tensor dimensions `n, h, w, c`.
- `cmsis_nn_tile` – width/height pair used for stride, padding, dilation.
- `cmsis_nn_conv_params` – input/output offsets (negated zero points), stride, padding, dilation and activation clamp.
- `cmsis_nn_per_channel_quant_params`, `cmsis_nn_per_tensor_quant_params`, `cmsis_nn_quant_params` – requantization
  multiplier and shift.
- `cmsis_nn_activation` – min/max clamp.

Float equivalents live in `arm_nn_types_flt.h` and are only pulled in when a float feature gate is on.

## 7. Configuration macros

| Macro | Where handled | Effect |
| --- | --- | --- |
| `ARM_NN_ENABLE_F32`, `ARM_NN_ENABLE_F16` | `Include/Internal/arm_nn_config.h`, CMake options | Enable the experimental float APIs and compile the float sources. Default off. |
| `NN_DISABLE_SPECIALIZATION` | build flag | Force generic implementations instead of shape-specific fast paths. |
| `ARM_NN_USE_EXP_LUT` / `ARM_NN_USE_EXP_TAYLOR` | `arm_nn_config.h` | Select scalar float softmax exp approximation. Defining both is a compile error. |
| `CMSIS_NN_USE_SINGLE_ROUNDING` | build flag | Single instead of double rounding in requantization; may change output. |
| `CMSIS_NN_USE_REQUANTIZE_INLINE_ASSEMBLY` | build flag | Inline-assembly `arm_nn_requantize`, faster on Cortex-M4. |
| `OPTIONAL_RESTRICT_KEYWORD` | build flag | Enable `restrict` on the output pointer in DSP int4/int8 convolutions; recommended for Cortex-M7. |
| `CMSIS_OPTIMIZATION_LEVEL` | CMake cache variable | Compiler optimisation level, default `-Ofast`. |

Options that affect headers must be mirrored in TFL/TFLM when used together with them.

## 8. Build structure

`CMakeLists.txt` creates a static library target `cmsis-nn`, adds `Include` as a public include directory and exports
`ARM_NN_ENABLE_F32` / `ARM_NN_ENABLE_F16` as public compile definitions. `Source/CMakeLists.txt` adds each operator
folder behind a per-category CMake variable and always adds `NNSupportFunctions` last. Each folder's `CMakeLists.txt`
collects files by type suffix, for example in `Source/ConvolutionFunctions/CMakeLists.txt`:

```cmake
file(GLOB SRC_S4  "./*_s4*.c")
file(GLOB SRC_S8  "./*_s8*.c")
file(GLOB SRC_S16 "./*_s16*.c")
# float sources are appended only when ARM_NN_ENABLE_F32 / ARM_NN_ENABLE_F16 are ON
```

A new integer kernel that follows the naming scheme is therefore picked up automatically. A new float kernel must be
added to the explicit list in that folder's `CMakeLists.txt`.
