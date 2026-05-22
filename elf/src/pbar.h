#pragma once

typedef enum {
    PBAR_TEXT_APPLY,
    PBAR_TEXT_SAVE,
    PBAR_TEXT_PLAIN,
} PBarTextType;

int PBar_Create();
void PBar_Step(PBarTextType type, const char *text);
