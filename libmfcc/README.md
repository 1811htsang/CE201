# libmfcc

`libmfcc` is a standalone C MFCC extractor for MCU and host builds. It has no HAL or CMSIS dependency, so an application can feed it samples from I2S DMA, a WAV file, or another audio source.

## Features

- Runtime configuration for sample rate, FFT, hop, Mel filters, MFCC count, frequency limits, log floor, input scale, and per-frame DC removal.
- Caller-owned workspace; no `malloc` is used by the library.
- `mfcc_compute_frame()` for a DMA frame and `mfcc_compute_frames()` for a contiguous sample buffer.
- Power-of-two radix-2 FFT and Type-II DCT.

## Build and test

```sh
cmake -S libmfcc -B libmfcc/build
cmake --build libmfcc/build
ctest --test-dir libmfcc/build --output-on-failure
```

## STM32 integration

Add `libmfcc/Source/mfcc.c` to the firmware target and add `libmfcc/Include` to its include paths. Before initialization, allocate a statically aligned workspace:

```c
mfcc_config_t config;
mfcc_context_t mfcc;
mfcc_default_config(&config);
config.fs_hz = 32000U;
config.n_fft = 1024U;
config.hop = 512U;
config.n_mels = 40U;
config.n_mfcc = 13U;
config.fmin_hz = 20.0f;
config.fmax_hz = 16000.0f;

/* Size this array with mfcc_workspace_size(&config) / sizeof(float). */
static float workspace[26000];
mfcc_init(&mfcc, &config, workspace, sizeof(workspace));
mfcc_compute_frame(&mfcc, frame_samples, mfcc_values);
```

In production firmware, use the value returned by `mfcc_workspace_size()` to set the workspace array size at compile time, or reserve a conservative buffer in the linker section used for DSP workspaces. `window_frames` is retained as
window metadata; `mfcc_compute_frames()` returns every complete frame in the provided sample range.
