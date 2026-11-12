#include "mfcc.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#define MFCC_PI 3.14159265358979323846f

static size_t align_up(size_t value)
{
    const size_t alignment = sizeof(float);
    return (value + alignment - 1U) & ~(alignment - 1U);
}

static float *take_buffer(uint8_t **cursor, size_t count)
{
    float *buffer = (float *)*cursor;
    *cursor += count * sizeof(float);
    return buffer;
}

static int is_power_of_two(uint32_t value)
{
    return value != 0U && (value & (value - 1U)) == 0U;
}

void mfcc_default_config(mfcc_config_t *config)
{
    if (config == NULL) {
        return;
    }

    config->fs_hz = 32000U;
    config->n_fft = 1024U;
    config->hop = 512U;
    config->n_mels = 40U;
    config->n_mfcc = 13U;
    config->fmin_hz = 20.0f;
    config->fmax_hz = 16000.0f;
    config->log_floor = 1.0e-6f;
    config->log_scale = 20.0f;
    config->input_scale = 1.0f;
    config->window_frames = 1U;
    config->dc_removal = 1U;
}

static int valid_config(const mfcc_config_t *config)
{
    return config != NULL && config->fs_hz > 0U &&
           is_power_of_two(config->n_fft) && config->n_fft >= 2U &&
           config->hop > 0U && config->n_mels > 0U &&
           config->n_mels + 2U <= config->n_fft && config->n_mfcc > 0U &&
           config->n_mfcc <= config->n_mels && config->fmin_hz >= 0.0f &&
           config->fmax_hz > config->fmin_hz &&
           config->fmax_hz <= (float)config->fs_hz * 0.5f &&
           config->log_floor > 0.0f && config->log_scale > 0.0f &&
           isfinite(config->input_scale);
}

size_t mfcc_workspace_size(const mfcc_config_t *config)
{
    size_t bins;
    size_t bytes;

    if (!valid_config(config)) {
        return 0U;
    }

    bins = (size_t)config->n_fft / 2U + 1U;
    bytes = align_up(0U);
    bytes += (size_t)config->n_fft * sizeof(float);
    bytes += (size_t)config->n_mels * bins * sizeof(float);
    bytes += (size_t)config->n_mfcc * config->n_mels * sizeof(float);
    bytes += (size_t)config->n_fft * 2U * sizeof(float);
    bytes += bins * sizeof(float);
    bytes += (size_t)config->n_mels * sizeof(float);
    bytes += (size_t)config->n_fft * sizeof(float);
    return align_up(bytes);
}

static float hz_to_mel(float hz)
{
    return 2595.0f * log10f(1.0f + hz / 700.0f);
}

static float mel_to_hz(float mel)
{
    return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f);
}

static void init_window(mfcc_context_t *context)
{
    uint32_t i;
    for (i = 0U; i < context->config.n_fft; ++i) {
        context->window[i] = 0.54f - 0.46f * cosf(
            2.0f * MFCC_PI * (float)i / (float)(context->config.n_fft - 1U));
    }
}

static void init_filterbank(mfcc_context_t *context)
{
    const mfcc_config_t *config = &context->config;
    uint32_t bins = config->n_fft / 2U + 1U;
    uint32_t points = config->n_mels + 2U;
    float mel_low = hz_to_mel(config->fmin_hz);
    float mel_high = hz_to_mel(config->fmax_hz);
    float *edges = context->frame;
    uint32_t i;

    memset(context->mel_filterbank, 0,
           (size_t)config->n_mels * bins * sizeof(float));
    for (i = 0U; i < points; ++i) {
        float mel = mel_low + (mel_high - mel_low) * (float)i /
                    (float)(points - 1U);
        float hz = mel_to_hz(mel);
        uint32_t edge = (uint32_t)floorf((float)(config->n_fft + 1U) * hz /
                                         (float)config->fs_hz);
        edges[i] = (float)(edge < bins ? edge : bins - 1U);
    }

    for (i = 0U; i < config->n_mels; ++i) {
        uint32_t left = (uint32_t)edges[i];
        uint32_t center = (uint32_t)edges[i + 1U];
        uint32_t right = (uint32_t)edges[i + 2U];
        uint32_t k;
        float *filter = context->mel_filterbank + (size_t)i * bins;

        for (k = left; k < center && k < bins; ++k) {
            if (center != left) {
                filter[k] = (float)(k - left) / (float)(center - left);
            }
        }
        for (k = center; k <= right && k < bins; ++k) {
            if (right != center) {
                filter[k] = (float)(right - k) / (float)(right - center);
            }
        }
    }
}

static void init_dct(mfcc_context_t *context)
{
    uint32_t n;
    for (n = 0U; n < context->config.n_mfcc; ++n) {
        uint32_t m;
        for (m = 0U; m < context->config.n_mels; ++m) {
            context->dct[(size_t)n * context->config.n_mels + m] = cosf(
                MFCC_PI * (float)n * (2.0f * (float)m + 1.0f) /
                (2.0f * (float)context->config.n_mels));
        }
    }
}

mfcc_status_t mfcc_init(mfcc_context_t *context,
                        const mfcc_config_t *config,
                        void *workspace,
                        size_t workspace_size)
{
    uint8_t *cursor;
    size_t required;
    size_t bins;

    if (context == NULL || config == NULL || workspace == NULL) {
        return MFCC_ERROR_ARGUMENT;
    }
    if (!valid_config(config)) {
        return MFCC_ERROR_CONFIG;
    }
    required = mfcc_workspace_size(config);
    if (workspace_size < required || ((uintptr_t)workspace % sizeof(float)) != 0U) {
        return MFCC_ERROR_WORKSPACE;
    }

    memset(context, 0, sizeof(*context));
    context->config = *config;
    context->workspace_size = required;
    cursor = (uint8_t *)workspace;
    bins = config->n_fft / 2U + 1U;
    context->window = take_buffer(&cursor, config->n_fft);
    context->mel_filterbank = take_buffer(&cursor, (size_t)config->n_mels * bins);
    context->dct = take_buffer(&cursor, (size_t)config->n_mfcc * config->n_mels);
    context->fft = take_buffer(&cursor, (size_t)config->n_fft * 2U);
    context->power = take_buffer(&cursor, bins);
    context->mel_energy = take_buffer(&cursor, config->n_mels);
    context->frame = take_buffer(&cursor, config->n_fft);

    init_window(context);
    init_filterbank(context);
    init_dct(context);
    return MFCC_OK;
}

static void fft(mfcc_context_t *context)
{
    uint32_t n = context->config.n_fft;
    uint32_t i;
    uint32_t j = 0U;
    for (i = 0U; i < n; ++i) {
        if (i < j) {
            float real = context->fft[2U * i];
            float imag = context->fft[2U * i + 1U];
            context->fft[2U * i] = context->fft[2U * j];
            context->fft[2U * i + 1U] = context->fft[2U * j + 1U];
            context->fft[2U * j] = real;
            context->fft[2U * j + 1U] = imag;
        }
        {
            uint32_t bit = n >> 1U;
            while (j & bit) {
                j ^= bit;
                bit >>= 1U;
            }
            j ^= bit;
        }
    }

    for (uint32_t length = 2U; length <= n; length <<= 1U) {
        uint32_t half = length >> 1U;
        float angle = -2.0f * MFCC_PI / (float)length;
        for (i = 0U; i < n; i += length) {
            for (j = 0U; j < half; ++j) {
                float wr = cosf(angle * (float)j);
                float wi = sinf(angle * (float)j);
                uint32_t even = 2U * (i + j);
                uint32_t odd = 2U * (i + j + half);
                float tr = wr * context->fft[odd] - wi * context->fft[odd + 1U];
                float ti = wr * context->fft[odd + 1U] + wi * context->fft[odd];
                float er = context->fft[even];
                float ei = context->fft[even + 1U];
                context->fft[even] = er + tr;
                context->fft[even + 1U] = ei + ti;
                context->fft[odd] = er - tr;
                context->fft[odd + 1U] = ei - ti;
            }
        }
    }
}

mfcc_status_t mfcc_compute_frame(mfcc_context_t *context,
                                 const float *samples,
                                 float *output)
{
    const mfcc_config_t *config;
    uint32_t i;
    uint32_t bins;

    if (context == NULL || samples == NULL || output == NULL) {
        return MFCC_ERROR_ARGUMENT;
    }
    config = &context->config;
    if (config->dc_removal) {
        float mean = 0.0f;
        for (i = 0U; i < config->n_fft; ++i) {
            mean += samples[i] * config->input_scale;
        }
        mean /= (float)config->n_fft;
        for (i = 0U; i < config->n_fft; ++i) {
            context->frame[i] = (samples[i] * config->input_scale - mean) *
                                context->window[i];
        }
    } else {
        for (i = 0U; i < config->n_fft; ++i) {
            context->frame[i] = samples[i] * config->input_scale * context->window[i];
        }
    }

    memset(context->fft, 0, (size_t)config->n_fft * 2U * sizeof(float));
    for (i = 0U; i < config->n_fft; ++i) {
        context->fft[2U * i] = context->frame[i];
    }
    fft(context);

    bins = config->n_fft / 2U + 1U;
    for (i = 0U; i < bins; ++i) {
        float real = context->fft[2U * i];
        float imag = context->fft[2U * i + 1U];
        context->power[i] = (real * real + imag * imag) /
                            (float)config->n_fft;
    }

    for (i = 0U; i < config->n_mels; ++i) {
        float sum = 0.0f;
        uint32_t k;
        const float *filter = context->mel_filterbank + (size_t)i * bins;
        for (k = 0U; k < bins; ++k) {
            sum += filter[k] * context->power[k];
        }
        context->mel_energy[i] = config->log_scale * log10f(
            sum > config->log_floor ? sum : config->log_floor);
    }

    for (i = 0U; i < config->n_mfcc; ++i) {
        float sum = 0.0f;
        uint32_t m;
        const float *row = context->dct + (size_t)i * config->n_mels;
        for (m = 0U; m < config->n_mels; ++m) {
            sum += row[m] * context->mel_energy[m];
        }
        output[i] = sum;
    }
    return MFCC_OK;
}

mfcc_status_t mfcc_compute_frames(mfcc_context_t *context,
                                  const float *samples,
                                  size_t sample_count,
                                  float *output,
                                  size_t output_frames,
                                  size_t *written_frames)
{
    size_t frames;
    size_t i;

    if (written_frames != NULL) {
        *written_frames = 0U;
    }
    if (context == NULL || samples == NULL || output == NULL ||
        written_frames == NULL) {
        return MFCC_ERROR_ARGUMENT;
    }
    if (sample_count < context->config.n_fft) {
        return MFCC_OK;
    }
    frames = 1U + (sample_count - context->config.n_fft) / context->config.hop;
    if (output_frames < frames) {
        return MFCC_ERROR_OUTPUT;
    }
    for (i = 0U; i < frames; ++i) {
        mfcc_status_t status = mfcc_compute_frame(
            context, samples + i * context->config.hop,
            output + i * context->config.n_mfcc);
        if (status != MFCC_OK) {
            return status;
        }
    }
    *written_frames = frames;
    return MFCC_OK;
}