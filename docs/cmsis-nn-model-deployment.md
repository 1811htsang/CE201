# Deploying the CE201 int8 model with CMSIS-NN

## 1. Status

- No trained model exists in the repo yet. Phases 4-5 of `docs/to-do-training.md` are still open, so there is no `model_int8.tflite` to convert.
- Only two things were compiled on this machine: a sample `xxd -i` output and `arm_convolve_wrapper_s8.c`, both with host gcc. The arm-none-eabi build, the TFLM integration code, and the STM32H723 board were not tested.
- The I2S capture path is only mapped to the PAL (`pal/arch/stm32h723`). Treat audio-to-inference on hardware as unverified until it is tested with a sensor connected.
- Target: STM32H723ZGTx (Cortex-M7 with DSP extension, no Helium/MVE) in `CE201/phy-prjecobj-h723`. The ESP32-S3 branch uses esp-nn and is not covered here.

## 2. What each piece does

| Piece | Produced by | Role |
| --- | --- | --- |
| `model_int8.tflite` | `src/quantize.py` (PC) | Flatbuffer with graph, int8 weights, quantization parameters |
| `model_int8_data.c` | `xxd -i` | Byte array holding the `.tflite` file. Data only, no logic |
| TensorFlow Lite Micro (TFLM) | Firmware | Interpreter: parses the model, allocates tensors, runs layers in order |
| CMSIS-NN | Firmware | int8 kernels (conv, depthwise conv, fully connected, pooling, softmax, add) that TFLM calls |
| Application code | Firmware | Feature extraction, copy input, invoke, read output |

CMSIS-NN does not read the model array. It only provides kernels. Running the model needs an interpreter (TFLM) to parse the array and call CMSIS-NN for each layer.

Writing the network by hand with direct CMSIS-NN calls is possible (see `lib-cmsisnn/Guide/UsageGuide.md`, section 4), but it is not recommended here. It requires extracting weights and quantization parameters from the flatbuffer manually. This matches the plan in `docs/to-do-training.md` Phase 6: keep the TFLM path and use CMSIS-NN only as the kernel library.

## 3. Step 1: Export a fully int8 `.tflite` (PC)

Set the converter so input and output are int8. The default TFLite converter produces float32 I/O, and the firmware would then have to quantize the input itself.

```python
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_data  # TRAIN split only
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8
open("model_int8.tflite", "wb").write(converter.convert())
```

Before embedding, re-evaluate the `.tflite` on the test split with `tf.lite.Interpreter`. Record the input and output quantization parameters, because the firmware needs them to quantize features and to read the score:

```python
interp = tf.lite.Interpreter(model_path="model_int8.tflite")
print(interp.get_input_details()[0]["quantization"])   # (scale, zero_point)
print(interp.get_output_details()[0]["quantization"])
```

## 4. Step 2: Convert to a C array

```bash
xxd -i model_int8.tflite > model_int8_data.c
```

This produces:

```c
unsigned char model_int8_tflite[] = { 0x18, 0x00, ... };
unsigned int model_int8_tflite_len = 18200;
```

Edit the generated file so the data is const (kept in flash) and aligned. Rename it to `.cc` because TFLM is C++. Keep the `extern` on the definitions so they match the header declarations; in C++ a namespace-scope `const` otherwise has internal linkage:

```cpp
#include "model_int8.h"

alignas(16) extern const unsigned char model_int8_tflite[] = { 0x18, 0x00, ... };
extern const unsigned int model_int8_tflite_len = 18200;
```

Declare the symbols in `model_int8.h`:

```cpp
#pragma once
extern const unsigned char model_int8_tflite[];
extern const unsigned int model_int8_tflite_len;
```

Re-run `xxd` every time the `.tflite` changes. Do not hand-edit the array contents.

## 5. Step 3: Firmware integration (TFLM with CMSIS-NN kernels)

Prerequisites:

- **TFLM source.** It is not in this checkout. `tensorflow/tensorflow/lite/micro/` contains only a README pointing to the standalone repository `tensorflow/tflite-micro`. Vendor it or add it as a submodule, and pin a version.
- **CMSIS-NN kernels enabled in TFLM.** `lib-cmsisnn/Guide/UsageGuide.md` section 1 states that LiteRT/TFLM calls CMSIS-NN once its CMSIS-NN kernels are enabled. Turn the option on in the TFLM build for the pinned version and confirm the exact flag in that version's documentation.
- **Only the ops the model uses.** Inspect the converted model in Netron and register only those ops in the resolver. A GlobalAveragePooling1D may appear as MEAN or AVERAGE_POOL_2D depending on the converter, so check the graph rather than assuming.

Example (C++, e.g. `app/inference/nn_infer.cc`). Replace the op list with what the model actually uses, and verify API names against the pinned TFLM version:

```cpp
#include <string.h>

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model_int8.h"

namespace {
constexpr int kArenaSize = 40 * 1024;  // measure with arena_used_bytes(), see Step 5
alignas(16) uint8_t g_arena[kArenaSize];
tflite::MicroInterpreter* g_interp = nullptr;
TfLiteTensor* g_in = nullptr;
TfLiteTensor* g_out = nullptr;
}  // namespace

extern "C" int nn_init(void) {
  static const tflite::Model* model = tflite::GetModel(model_int8_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) return -1;

  static tflite::MicroMutableOpResolver<4> resolver;
  resolver.AddConv2D();
  resolver.AddFullyConnected();
  resolver.AddAveragePool2D();
  resolver.AddSoftmax();

  static tflite::MicroInterpreter interp(model, resolver, g_arena, kArenaSize);
  if (interp.AllocateTensors() != kTfLiteOk) return -2;

  g_interp = &interp;
  g_in = interp.input(0);
  g_out = interp.output(0);
  return 0;
}

extern "C" int nn_infer(const int8_t* features, int8_t* scores) {
  memcpy(g_in->data.int8, features, g_in->bytes);
  if (g_interp->Invoke() != kTfLiteOk) return -3;
  memcpy(scores, g_out->data.int8, g_out->bytes);
  return 0;
}
```

Notes:

- `extern "C"` lets the uEDP C code call the wrapper. Keep all TFLM types inside the `.cc` file.
- `features` must already be int8 and quantized with the input scale and zero point from Step 1. The feature extractor (CMSIS-DSP MFCC) is a separate task.
- Confirm that `g_in->bytes` equals the feature vector size at init, not only at runtime.

## 6. Step 4: Build

### 6.1 CMSIS-NN for Cortex-M7

Option A, CMake static library (run from the `pwork` root; the Windows paths may need adjusting for your shell):

```bash
cmake -S CE201/lib-cmsisnn -B build-h723 \
  -DCMAKE_SYSTEM_NAME=Generic \
  -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMSISNN_TARGET_CPU=cortex-m7 \
  -DCMSISNN_USE_CMSIS_CORE=ON \
  -DCMSISNN_CMSIS_CORE_PATH=CE201/phy-prjecobj-h723/Drivers/CMSIS/Include
cmake --build build-h723
```

Copy the `-mfpu` and `-mfloat-abi` flags from the STM32CubeIDE project settings. The CMake option does not add them. Mismatched float ABI between CMSIS-NN, TFLM, and the application causes link errors.

Option B, add sources directly to the STM32CubeIDE project. Add `lib-cmsisnn/Source/**/*.c` and `lib-cmsisnn/Include` (`lib-cmsisnn/Guide/UsageGuide.md` section 1). Do not add `Source/Bindings`, which belongs to the host-side Python extension (`lib-cmsisnn/Guide/BuildAndTest.md`).

Compiler flags: keep CMSIS-NN's default `-Ofast`. Avoid `-fno-builtin` and `-ffreestanding`, which disable optimized `memcpy`/`memset` that CMSIS-NN depends on.

### 6.2 Toolchain

`arm-none-eabi-gcc` is not installed on this machine. The MSYS2 gcc only builds for the host. Install the toolchain either through STM32CubeIDE (its `.cproject` references `gnu_tools_for_stm32`) or as the standalone Arm GNU Toolchain.

## 7. Step 5: Validate

1. **Reference outputs (PC).** Generate a set of int8 feature vectors on the PC and run them through `tf.lite.Interpreter`. Save the expected outputs.
2. **Same vectors on target.** Feed the same inputs to `nn_infer` (through a debugger memory write, UART, or a test harness) and compare outputs. `lib-cmsisnn/README.md` states CMSIS-NN is bit-exact with the TFLM reference kernels. Python `tf.lite` runs the TFLite reference kernels, which the README notes can differ from TFLM on some ops. Expect the int8 outputs to match; if they do not, check the quantization parameters first, then the op-level differences.
3. **Arena size.** After `AllocateTensors()`, read `g_interp->arena_used_bytes()` and set `kArenaSize` to that value plus a small margin. CMSIS-NN scratch buffers come from the TFLM arena, so measure rather than estimate.
4. **Footprint.** Run `arm-none-eabi-size` on the ELF to get flash (text + rodata) and RAM (data + bss) use.
5. **Latency.** Time `nn_infer` with the DWT cycle counter. The benchmark table (Architecture A4) should use this method.

## 8. Environment check (this machine)

| Item | Status | Action |
| --- | --- | --- |
| Python | 3.12 at `C:/msys64/ucrt64/bin/python.exe`, no pip | Install a Python 3.12 with pip (python.org or the `py` launcher), then create a venv |
| TensorFlow | Not installed | Install into the venv. Pinned versions are in `tensorflow/requirements_lock_3_12.txt` |
| TensorFlow import path | Running Python from `pwork/` imports the local `tensorflow/` source folder, not the package | Run training scripts from outside `pwork/` |
| gcc | MSYS2 gcc 15.2.0 | Host tests only |
| xxd | Available | None |
| cmake | 4.4.3 | None |
| arm-none-eabi-gcc | Missing | Install via STM32CubeIDE or Arm GNU Toolchain |
| TFLM source | Missing | Vendor or add as a pinned submodule |

## 9. Open issues to resolve first

1. **Feature parameters conflict.** `docs/review/model-training-pipeline.md` specifies 16 kHz, `n_fft` 512, `hop` 256, 61 frames per window (about 1 s). `docs/to-do-training.md` specifies 32 kHz, `n_fft` 1024, `hop` 512, 311 frames per window (about 5 s). The firmware input tensor shape and feature extractor depend on which one is used. Choose one and update both documents.
2. **I2S path not validated on hardware.** Capture-to-inference has only been checked at the PAL mapping level.
3. **TFLM version not pinned.** Choose and record a version before writing the firmware integration.
4. **Artifact naming.** The pipeline doc names the output `model_int8.h`. This guide uses `model_int8_data.c` for the array and `model_int8.h` for the extern declarations. Keep these names consistent in `src/quantize.py`.
