#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MergeIMGHDR ((void (*)(IMGHDR *, IMGHDR *))(0xa094d0dc | 1))

#define RGB565_TRANSPARENT_COLOR 0xE000

static void RGB565_to_ARGB8888(uint8_t dest[4], uint16_t pixel) {
    if (pixel == RGB565_TRANSPARENT_COLOR) {
        memset(dest, 0, 4);
        return;
    }
    uint8_t r = (pixel >> 11) & 0x1F;
    uint8_t g = (pixel >> 5) & 0x3F;
    uint8_t b = pixel & 0x1F;
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
    dest[0] = b;
    dest[1] = g;
    dest[2] = r;
    dest[3] = 0xFF;
}

static uint16_t ARGB8888_to_RGB565(const uint8_t src[4]) {
    if (src[3] == 0) {
        return RGB565_TRANSPARENT_COLOR;
    }
    const uint8_t r = src[2] >> 3;
    const uint8_t g = src[1] >> 2;
    const uint8_t b = src[0] >> 3;
    return (r << 11) | (g << 5) | b;
}

static void BlendPixel(uint8_t dest_pixel[4], const uint8_t src_pixel[4]) {
    const uint8_t alpha = src_pixel[3];
    if (alpha == 0) { // transparent
    } else if (alpha == 0xFF) {
        dest_pixel[0] = src_pixel[0];
        dest_pixel[1] = src_pixel[1];
        dest_pixel[2] = src_pixel[2];
        dest_pixel[3] = 0xFF;
    } else {
        const uint8_t inv = 0xFF - alpha;
        dest_pixel[0] = (src_pixel[0] * alpha + dest_pixel[0] * inv) / 0xFF;
        dest_pixel[1] = (src_pixel[1] * alpha + dest_pixel[1] * inv) / 0xFF;
        dest_pixel[2] = (src_pixel[2] * alpha + dest_pixel[2] * inv) / 0xFF;
        dest_pixel[3] = alpha + (dest_pixel[3] * inv) / 0xFF;
    }
}

static void NormalizeRange(uint16_t *start, uint16_t *end) {
    if (*start > *end) {
        uint16_t t = *start;
        *start = *end;
        *end = t;
    }
}

static void ConvertPhoneColor(uint8_t dest[4], const uint8_t src[4]) {
    memcpy(dest, src, 4);
    const int alpha = dest[3];
    dest[3] = (alpha * 0xFF + 50) / 0x64;
}

static int IMGHDR_IsValid(const IMGHDR *img) {
    return (img && img->bitmap && img->bpnum == IMGHDR_TYPE_ARGB8888);
}

int IMGHDR_Clone(IMGHDR *dest, const IMGHDR *src) {
    if (!dest || !src || !src->bitmap || dest == src) {
        return 0;
    }
    const size_t size = CalcBitmapSize((short)src->w, (short)src->h, src->bpnum);
    uint8_t *dest_b = malloc(size);
    if (!dest_b) {
        return 0;
    }
    if (dest->bitmap) {
        mfree(dest->bitmap);
    }
    memcpy(dest_b, src->bitmap, size);
    memcpy(dest, src, sizeof(IMGHDR));
    dest->bitmap = dest_b;
    return 1;
}

IMGHDR *IMGHDR_ARGB8888_to_RGB565(const IMGHDR *src) {
    if (!IMGHDR_IsValid(src)) {
        return NULL;
    }
    IMGHDR *img = malloc(sizeof(IMGHDR));
    if (img) {
        memcpy(img, src, sizeof(IMGHDR));
        img->bpnum = IMGHDR_TYPE_RGB565;
        const size_t size = CalcBitmapSize((short)img->w, (short)img->h, img->bpnum);
        img->bitmap = malloc(size);
        if (!img->bitmap) {
            mfree(img);
            return NULL;
        }

        uint16_t *dest_b = (uint16_t*)img->bitmap;
        const uint8_t *src_b = src->bitmap;
        for (int i = 0; i < img->w * img->h; i++) {
            dest_b[i] = ARGB8888_to_RGB565(src_b + i * 4);
        }
    }
    return img;
}

IMGHDR *IMGHDR_CropVertical(const IMGHDR *src, uint16_t crop_y, uint16_t crop_h) {
    if (!src || !src->bitmap || src->bpnum != IMGHDR_TYPE_RGB565) {
        return NULL;
    }
    if (crop_y + crop_h > src->h || crop_h == 0) {
        return NULL;
    }
    IMGHDR *img = malloc(sizeof(IMGHDR));
    if (img) {
        memcpy(img, src, sizeof(IMGHDR));
        img->h = crop_h;
        const size_t size = CalcBitmapSize((short)img->w, (short)img->h, IMGHDR_TYPE_ARGB8888);
        img->bitmap = malloc(size);
        if (!img->bitmap) {
            mfree(img);
            return NULL;
        }
        img->bpnum = IMGHDR_TYPE_ARGB8888;
        for (int i = 0; i < img->h * src->w; i++) {
            uint8_t *dest_b = img->bitmap + i * 4;
            uint8_t *src_b = src->bitmap + (crop_y * src->w + i) * 2;
            RGB565_to_ARGB8888(dest_b, *(uint16_t*)src_b);
        }
    }
    return img;
}

IMGHDR *IMGHDR_CreateSolid(uint16_t w, uint16_t h, const uint8_t rgba[4]) {
    IMGHDR *img = malloc(sizeof(IMGHDR));
    if (img) {
        img->w = w;
        img->h = h;
        img->bpnum = IMGHDR_TYPE_ARGB8888;
        const size_t size = CalcBitmapSize((short)w, (short)h, IMGHDR_TYPE_ARGB8888);
        img->bitmap = malloc(size);
        if (!img->bitmap) {
            mfree(img);
            return NULL;
        }
        uint8_t color[4];
        ConvertPhoneColor(color, rgba);
        for (int i = 0; i < w * h; i++) {
            uint8_t *pixel = img->bitmap + i * 4;
            pixel[0] = color[2];
            pixel[1] = color[1];
            pixel[2] = color[0];
            pixel[3] = color[3];
        }
    }
    return img;
}

IMGHDR *IMGHDR_CreateGlowMask(uint16_t w, uint16_t h, const uint8_t rgba[4]) {
    IMGHDR *mask = malloc(sizeof(IMGHDR));
    mask->w = w;
    mask->h = h;
    mask->bpnum = IMGHDR_TYPE_ARGB8888;
    mask->bitmap = malloc(w * h * 4);
    if (!mask->bitmap) {
        mfree(mask);
        return NULL;
    }

    uint8_t color[4];
    ConvertPhoneColor(color, rgba);
    const float cx = w / 2.0f;
    const float cy = h / 2.0f;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const float dx = (x - cx) / cx;
            const float dy = (y - cy) / cy;
            const float dist = sqrtf(dx * dx + dy * dy);
            float alphaFactor = 1.0f - dist * dist;
            if (alphaFactor < 0) alphaFactor = 0;
            const uint8_t alpha = (uint8_t)(alphaFactor * color[3]);
            uint8_t *pixel = mask->bitmap + (y * w + x) * 4;
            pixel[0] = color[2];
            pixel[1] = color[1];
            pixel[2] = color[0];
            pixel[3] = alpha;
        }
    }
    return mask;
}

void IMGHDR_Blend(IMGHDR *dest, const IMGHDR *overlay) {
    if (!IMGHDR_IsValid(dest) || !IMGHDR_IsValid(overlay)) {
        return;
    }
    if (dest->w != overlay->w || dest->h != overlay->h) {
        return;
    }
    for (int i = 0; i < dest->w * dest->h; i++) {
        BlendPixel(dest->bitmap + i * 4, overlay->bitmap + i * 4);
    }
}

void IMGHDR_DrawHLine(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t x2, int flags, const uint8_t rgba[4]) {
    if (!IMGHDR_IsValid(dest)) {
        return;
    }
    uint16_t start = x;
    uint16_t end = x2;
    NormalizeRange(&start, &end);
    if (start == end) {
        return;
    }
    if (start >= dest->w || end >= dest->w || y >= dest->h) {
        return;
    }
    if (rgba[3] == 0x00) {
        return;
    }
    uint8_t color[4];
    ConvertPhoneColor(color, rgba);
    const int count = end - start + 1;
    uint8_t *pixel = dest->bitmap + (y * dest->w + start) * 4;
    for (int i = 0; i < count; i++) {
        if (flags & LINE_DOTTED && (i % 2) != 0) {
            continue;
        }
        BlendPixel(pixel + i * 4, (uint8_t[4]){color[2], color[1], color[0], color[3]});
    }
}

void IMGHDR_DrawVLine(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t y2, int flags, const uint8_t rgba[4]) {
    if (!IMGHDR_IsValid(dest)) {
        return;
    }
    uint16_t start = y;
    uint16_t end = y2;
    NormalizeRange(&start, &end);
    if (start == end) {
        return;
    }
    if (start >= dest->h || end >= dest->h || x >= dest->w) {
        return;
    }
    if (rgba[3] == 0x00) {
        return;
    }
    uint8_t color[4];
    ConvertPhoneColor(color, rgba);
    const int count = end - start + 1;
    uint8_t *pixel = dest->bitmap + (start * dest->w + x) * 4;
    for (int i = 0; i < count; i++, pixel += dest->w * 4) {
        if (flags & LINE_DOTTED && (i % 2) != 0) {
            continue;
        }
        BlendPixel(pixel, (uint8_t[4]){color[2], color[1], color[0], color[3]});
    }
}

void IMGHDR_DrawBorder(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, int flags, const uint8_t rgba[4]) {
    IMGHDR_DrawHLine(dest, x, y, x2, flags, rgba);
    IMGHDR_DrawHLine(dest, x, y2, x2, flags, rgba);
    IMGHDR_DrawVLine(dest, x, y + 1, y2 - 1, flags, rgba);
    IMGHDR_DrawVLine(dest, x2, y + 1, y2 - 1, flags, rgba);
}

void IMGHDR_BoxBlur(IMGHDR *dest, int radius) {
    if (!IMGHDR_IsValid(dest)) {
        return;
    }
    if (radius <= 0) {
        return;
    }
    const int w = dest->w;
    const int h = dest->h;
    const size_t size = (size_t)w * h * 4;
    uint8_t *tmp = malloc(size);
    if (!tmp) {
        return;
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum[4] = {0};
            int count = 0;
            for (int dx = -radius; dx <= radius; dx++) {
                int nx = x + dx;
                if (nx < 0 || nx >= w) continue;
                uint8_t *p = dest->bitmap + (y * w + nx) * 4;
                sum[0] += p[0];
                sum[1] += p[1];
                sum[2] += p[2];
                sum[3] += p[3];
                count++;
            }
            uint8_t *d = tmp + (y * w + x) * 4;
            d[0] = sum[0] / count;
            d[1] = sum[1] / count;
            d[2] = sum[2] / count;
            d[3] = sum[3] / count;
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum[4] = {0};
            int count = 0;
            for (int dy = -radius; dy <= radius; dy++) {
                int ny = y + dy;
                if (ny < 0 || ny >= h) continue;
                uint8_t *p = tmp + (ny * w + x) * 4;
                sum[0] += p[0];
                sum[1] += p[1];
                sum[2] += p[2];
                sum[3] += p[3];
                count++;
            }
            uint8_t *d = dest->bitmap + (y * w + x) * 4;
            d[0] = sum[0] / count;
            d[1] = sum[1] / count;
            d[2] = sum[2] / count;
            d[3] = sum[3] / count;
        }
    }
    free(tmp);
}

void IMGHDR_TransparentV(IMGHDR *dest, uint16_t y, uint16_t y2) {
    if (!IMGHDR_IsValid(dest)) {
        return;
    }
    uint16_t start = y;
    uint16_t end = y2;
    NormalizeRange(&start, &end);
    if (start >= dest->h || end >= dest->h) {
        return;
    }
    for (int i = start; i <= end; i++) {
        uint8_t *row = dest->bitmap + (i * dest->w) * 4;
        for (int x = 0; x < dest->w; x++) {
            row[x * 4 + 3] = 0x00;
        }
    }
}

void IMGHDR_TransparentH(IMGHDR *dest, uint16_t x, uint16_t x2) {
    if (!IMGHDR_IsValid(dest)) {
        return;
    }
    uint16_t start = x;
    uint16_t end = x2;
    NormalizeRange(&start, &end);
    if (start >= dest->w || end >= dest->w) {
        return;
    }
    for (int y = 0; y < dest->h; y++) {
        uint8_t *row = dest->bitmap + (y * dest->w) * 4;
        for (int i = start; i <= end; i++) {
            row[i * 4 + 3] = 0x00;
        }
    }
}

void FreeIMGHDR(IMGHDR *img) {
    if (img) {
        mfree(img->bitmap);
        mfree(img);
    }
}
