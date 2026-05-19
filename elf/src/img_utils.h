#pragma once

#include <swilib.h>

int IMGHDR_Clone(IMGHDR *dest, const IMGHDR *src);
IMGHDR *IMGHDR_ARGB8888_to_RGB565(const IMGHDR *src);
IMGHDR *IMGHDR_CropVertical(const IMGHDR *src, uint16_t crop_y, uint16_t crop_h);
IMGHDR *IMGHDR_CreateSolid(uint16_t w, uint16_t h, const uint8_t rgba[4]);
IMGHDR *IMGHDR_CreateGlowMask(uint16_t w, uint16_t h, const uint8_t rgba[4]);
void IMGHDR_Blend(IMGHDR *dest, const IMGHDR *overlay);
void IMGHDR_DrawHLine(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t x2, int flags, const uint8_t rgba[4]);
void IMGHDR_DrawVLine(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t y2, int flags, const uint8_t rgba[4]);
void IMGHDR_DrawBorder(IMGHDR *dest, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, int flags, const uint8_t rgba[4]);
void IMGHDR_BoxBlur(IMGHDR *dest, int radius);
void IMGHDR_TransparentV(IMGHDR *dest, uint16_t y, uint16_t y2);
void IMGHDR_TransparentH(IMGHDR *dest, uint16_t x, uint16_t x2);
void FreeIMGHDR(IMGHDR *img);
