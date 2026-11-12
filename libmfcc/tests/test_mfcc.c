#include "mfcc.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    mfcc_config_t config;
    mfcc_context_t context;
    size_t workspace_size;
    void *workspace;
    float samples[128];
    float output[13 * 3];
    size_t written;
    size_t i;

    mfcc_default_config(&config);
    config.fs_hz = 8000U;
    config.n_fft = 64U;
    config.hop = 32U;
    config.n_mels = 20U;
    config.n_mfcc = 13U;
    config.fmax_hz = 4000.0f;

    workspace_size = mfcc_workspace_size(&config);
    assert(workspace_size > 0U);
    workspace = malloc(workspace_size);
    assert(workspace != NULL);
    assert(mfcc_init(&context, &config, workspace, workspace_size) == MFCC_OK);

    for (i = 0U; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        samples[i] = sinf(2.0f * 3.14159265358979323846f * 440.0f *
                          (float)i / (float)config.fs_hz);
    }
    assert(mfcc_compute_frames(&context, samples, 128U, output, 3U, &written) == MFCC_OK);
    assert(written == 3U);
    for (i = 0U; i < written * config.n_mfcc; ++i) {
        assert(isfinite(output[i]));
    }

    free(workspace);
    puts("MFCC test passed");
    return 0;
}