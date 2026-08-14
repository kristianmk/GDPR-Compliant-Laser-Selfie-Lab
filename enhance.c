extern unsigned char __heap_base;

static float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static float max3(float a, float b, float c) {
    float m = a > b ? a : b;
    return m > c ? m : c;
}

static float min3(float a, float b, float c) {
    float m = a < b ? a : b;
    return m < c ? m : c;
}

__attribute__((visibility("default")))
unsigned int buffer_ptr(void) {
    return (unsigned int)&__heap_base;
}

__attribute__((visibility("default")))
void enhance(unsigned int ptr,
             unsigned int pixel_count,
             float exposure_factor,
             float highlights,
             float shadows,
             float whites,
             float contrast,
             float vibrance,
             float intensity) {
    unsigned char *p = (unsigned char *)ptr;

    for (unsigned int i = 0; i < pixel_count; i++) {
        unsigned int o = i * 4u;
        float or_ = p[o] / 255.0f;
        float og  = p[o + 1u] / 255.0f;
        float ob  = p[o + 2u] / 255.0f;

        float r = or_ * exposure_factor;
        float g = og  * exposure_factor;
        float b = ob  * exposure_factor;

        float luma = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        luma = clamp01(luma);

        float shadow_mask = (1.0f - luma) * (1.0f - luma);
        float highlight_mask = luma * luma;

        /* Positive shadows opens dark tones. Negative highlights recovers bright tones. */
        float shadow_delta = shadows * shadow_mask * 0.34f;
        float highlight_delta = highlights * highlight_mask * 0.30f;
        r += shadow_delta + highlight_delta;
        g += shadow_delta + highlight_delta;
        b += shadow_delta + highlight_delta;

        /* Whites adds clean brightness mostly to the upper tonal range. */
        r += whites * highlight_mask * (1.0f - r) * 0.55f;
        g += whites * highlight_mask * (1.0f - g) * 0.55f;
        b += whites * highlight_mask * (1.0f - b) * 0.55f;

        /* Gentle global contrast around middle gray. */
        float cf = 1.0f + contrast * 0.55f;
        r = 0.5f + (r - 0.5f) * cf;
        g = 0.5f + (g - 0.5f) * cf;
        b = 0.5f + (b - 0.5f) * cf;

        r = clamp01(r);
        g = clamp01(g);
        b = clamp01(b);

        /* Vibrance protects already-saturated colors more than plain saturation. */
        float y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        float sat = max3(r, g, b) - min3(r, g, b);
        float vf = 1.0f + vibrance * (1.0f - sat) * 0.85f;
        r = clamp01(y + (r - y) * vf);
        g = clamp01(y + (g - y) * vf);
        b = clamp01(y + (b - y) * vf);

        /* Blend against the original so one Intensity slider controls the whole effect. */
        r = or_ + (r - or_) * intensity;
        g = og  + (g - og)  * intensity;
        b = ob  + (b - ob)  * intensity;

        p[o]      = (unsigned char)(clamp01(r) * 255.0f + 0.5f);
        p[o + 1u] = (unsigned char)(clamp01(g) * 255.0f + 0.5f);
        p[o + 2u] = (unsigned char)(clamp01(b) * 255.0f + 0.5f);
        /* Alpha p[o + 3] is intentionally preserved. */
    }
}
