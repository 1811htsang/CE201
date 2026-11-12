#ifndef MFCC_H
#define MFCC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MFCC_OK = 0,
    MFCC_ERROR_ARGUMENT = -1,
    MFCC_ERROR_WORKSPACE = -2,
    MFCC_ERROR_CONFIG = -3,
    MFCC_ERROR_OUTPUT = -4
} mfcc_status_t;

typedef struct {
    uint32_t fs_hz;
    uint32_t n_fft;
    uint32_t hop;
    uint32_t n_mels;
    uint32_t n_mfcc;
    float fmin_hz;
    float fmax_hz;
    float log_floor;
    float log_scale;
    float input_scale;
    uint32_t window_frames;
    uint8_t dc_removal;
} mfcc_config_t;

typedef struct {
    mfcc_config_t config;
    float *window;
    float *mel_filterbank;
    float *dct;
    float *fft;
    float *power;
    float *mel_energy;
    float *frame;
    size_t workspace_size;
} mfcc_context_t;

void mfcc_default_config(mfcc_config_t *config);
size_t mfcc_workspace_size(const mfcc_config_t *config);
mfcc_status_t mfcc_init(mfcc_context_t *context,
                        const mfcc_config_t *config,
                        void *workspace,
                        size_t workspace_size);

mfcc_status_t mfcc_compute_frame(mfcc_context_t *context,
                                 const float *samples,
                                 float *output);

mfcc_status_t mfcc_compute_frames(mfcc_context_t *context,
                                  const float *samples,
                                  size_t sample_count,
                                  float *output,
                                  size_t output_frames,
                                  size_t *written_frames);

#ifdef __cplusplus
}
#endif

#endif