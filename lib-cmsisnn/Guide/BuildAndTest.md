# Building and Testing CMSIS-NN

## Build the library

The library supports two include modes. The default standalone mode does not
require CMSIS-Core. Enable CMSIS-Core mode only when your application needs
those headers and provide its path explicitly.

```Bash
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux

cmake -S . -B build-stm32 \
  -DCMAKE_SYSTEM_NAME=Generic \
  -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMSISNN_TARGET_CPU=cortex-m4 \
  -DCMSISNN_USE_CMSIS_CORE=ON \
  -DCMSISNN_CMSIS_CORE_PATH=/path/to/CMSIS
cmake --build build-stm32
```

Useful options:

| Option | Default | Effect |
| --- | --- | --- |
| `-DCMSIS_OPTIMIZATION_LEVEL=<flag>` | `-Ofast` | Compiler optimisation level. With `-O0` on Helium targets, define `ARM_MATH_AUTOVECTORIZE`. |
| `-DCMSISNN_USE_CMSIS_CORE=ON` | OFF | Add CMSIS-Core headers to the CMSIS-NN target. |
| `-DCMSISNN_CMSIS_CORE_PATH=<path>` | empty | CMSIS-Core root or direct `Include` directory; required when CMSIS-Core is enabled. |
| `-DARM_NN_ENABLE_F32=ON` | OFF | Build float32 kernels. |
| `-DARM_NN_ENABLE_F16=ON` | OFF | Build float16 kernels (needs toolchain and target support). |
| `-DCMSISNN_BUILD_PYBIND=ON` | OFF | Build the `cmsis_nn` Python extension. |

Avoid `-fno-builtin` and `-ffreestanding`; they disable optimised `memcpy`/`memset`, which CMSIS-NN uses heavily.
Tested compilers are Arm Compiler 6 and Arm GNU Toolchain. Host builds are not supported out of the box.

## Python bindings

The extension exposes the host-side buffer-size getters (convolution wrapper, depthwise wrapper, fully connected,
average pool, SVDF, transpose convolution) together with `Backend`, `DataType` and `CortexM` enums.

```Bash
cmake -S . -B build -DCMSISNN_BUILD_PYBIND=ON
cmake --build build
pip wheel . -w dist
```

The CMake build also creates a shared library `cmsis-nn-shared` so the bindings can be tested with ctest. Python tests
are in `Tests/Bindings/` (one `test_*_buffer_size.py` per binding plus `test_bindings_common.py`).

## Unit tests

Location: `Tests/UnitTest/`. Framework: Unity. Targets: Arm Mbed OS hardware or the Arm Corstone-300 FVP.

- **Per-function test cases** are under `TestCases/test_<function name>/`, each with a `Unity/` entry file. Names
  mirror the function under test, e.g. `test_arm_convolve_s8`. Network-level tests also exist
  (`test_arm_ds_cnn_s_s8`, `test_arm_ds_cnn_l_s8`, float `ds_cnn_s_body`).
- **Test data** in `TestCases/TestData/` is generated, not hand-written. Integer data comes from
  `generate_test_data.py` (TensorFlow/TFLite based) and the newer scripts in `RefactoredTestGen/`. Float data comes from
  the PyTorch-based `*_settings_flt.py` scripts.
- **Float coverage** is tracked in `float_unit_test_coverage.md`, with `float_unit_test_coverage.yml` as the source of
  truth. The float FVP flow uses CMSIS-Toolbox, configured under `Tests/UnitTest/cmsis/`.
- **Bit-exactness:** LSTM and SVDF are bit-exact only against TFLM, so their data generation needs the `tflite_micro`
  interpreter.

Typical integer flow on the FVP:

```Python
python3 run_integer_unit_tests.py --tests all --build-fvp --run-fvp --toolchains GCC \
  --cmsis-pack-root ~/.cache/arm/packs --fvp-bin FVP_Corstone_SSE-300
```

Or manually, with `CMSIS_PATH` pointing to the CMSIS-Core startup files:

```Bash
cd Tests/UnitTest && mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path>/arm-none-eabi-gcc.cmake -DTARGET_CPU=cortex-m55 -DCMSIS_PATH=<CMSIS>
make test_arm_depthwise_conv_s8_opt
```

Format Python test scripts with yapf (`column_limit:120`, `indent_width:4`).

## CI workflows (`.github/workflows/`)

| Workflow | Purpose |
| --- | --- |
| `clang-format.yml` | C/C++ formatting check (`.clang-format`) |
| `integer-fvp.yml` | Integer unit tests on the Corstone-300 FVP |
| `float-fvp.yml`, `float-host.yml` | Float unit tests on the FVP and on a host |
| `lib-variants.yml` | Library variant matrix (toolchains GCC and AC6, default CPU cortex-m55, `-Ofast`) |
| `pack.yml`, `pdsc.yml` | CMSIS-Pack build and PDSC checks |

Helper scripts at the root: `check_pdsc.sh`, `check_version_and_date.sh`, `gen_pack.sh`.

## Contribution checklist

1. Follow the style of the file you edit (lower-case names with underscores, no Hungarian notation).
2. New function: one function per file, file name equal to function name, with a full Doxygen header on the prototype.
3. Update the version and date fields in every file you change (Semantic Versioning); `check_version_and_date.sh`
   verifies this.
4. Add or update unit tests for new features and bug fixes.
5. For a new float kernel, add it to the explicit source list in the folder's `CMakeLists.txt`.
