#pragma once

typedef enum TextTransform {
    TEXT_TRANSFORM_NONE,
    TEXT_TRANSFORM_CAPITALIZE,
    TEXT_TRANSFORM_UPPERCASE,
    TEXT_TRANSFORM_LOWERCASE,
    TEXT_TRANSFORM_TOTAL,
} TextTransform;

typedef struct {
    int enable;
    int font;
    int v_offset;
    TextTransform text_transform;
    int show_middle_icon;
} SOFT_KEYS;

typedef struct {
    SOFT_KEYS soft_keys;
} SETTINGS;
