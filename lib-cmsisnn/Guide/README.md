# CMSIS-NN Source Guide

This folder contains developer-oriented notes on the CMSIS-NN source tree. They complement the user-facing
[README](../../README.md) and the Doxygen API reference by describing how the code is organised and how to work in it.

| Document | Contents |
| --- | --- |
| [UsageGuide.md](./UsageGuide.md) | How to integrate CMSIS-NN, the calling pattern, C examples (conv, fully connected, pooling, softmax) and the Python buffer-size helpers |
| [Architecture.md](./Architecture.md) | Repository layout, naming conventions, architecture dispatch, buffer-size API, configuration macros |
| [ModuleReference.md](./ModuleReference.md) | Per-directory overview of the kernels under `Source/` and the headers under `Include/` |
| [BuildAndTest.md](./BuildAndTest.md) | Building the library, Python bindings, unit tests and CI workflows |

## Repository at a glance

```directory
cmsis-nn/
├── Include/            Public headers (arm_nn*.h) and Internal/ headers
├── Source/             Kernel implementations, one sub-folder per operator family
│   └── Bindings/       pybind11 C++ sources for the optional Python module
├── Tests/
│   ├── UnitTest/       Unity based C unit tests, test-data generators, Corstone-300 / CMSIS-Toolbox setups
│   └── Bindings/       Python tests for the pybind11 module
├── python/cmsis_nn/    Python package wrapper
├── Examples/           Example applications
├── .github/workflows/  CI (clang-format, float host/FVP, integer FVP, lib variants, pack, pdsc)
├── CMakeLists.txt      Top-level build (static library `cmsis-nn`, optional pybind11 module)
└── ARM.CMSIS-NN.pdsc   CMSIS-Pack description
```

Source: `Source/` holds 14 sub-folders and a `Bindings/` folder with 9 C++/header files.
