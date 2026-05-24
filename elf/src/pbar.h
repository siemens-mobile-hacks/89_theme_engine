#pragma once

typedef enum {
    PBAR_TEXT_APPLIED,
    PBAR_TEXT_SAVED,
    PBAR_TEXT_DELETED,
    PBAR_TEXT_PLAIN,
} PBarTextType;

int PBar_Create();
void PBar_Step(PBarTextType type, const char *text);
