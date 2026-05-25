#include <swilib.h>
#include "../../settings.h"

#ifdef E71_45
    #include "../E71_45.h"
#endif

#define _DrawString ((void (*)(const WSHDR *, int, int, int, int, int, int, const char *, const char *))(ADDR_DrawString))
#define _wstrtoupper ((void (*)(WSHDR *))(ADDR_wstrtoupper))
#define _wstrtolower ((void (*)(WSHDR *))(ADDR_wstrtolower))
#define _wstrcapitalize ((void (*)(WSHDR *))(ADDR_wstrcapitalize))

__attribute__((target("thumb")))
__attribute__((section(".text.TransformText")))
void TransformText(WSHDR *ws, TextTransform transform) {
    if (transform == TEXT_TRANSFORM_CAPITALIZE) {
        _wstrcapitalize(ws);
    } else if (transform == TEXT_TRANSFORM_UPPERCASE) {
        _wstrtoupper(ws);
    } else if (transform == TEXT_TRANSFORM_LOWERCASE) {
        _wstrtolower(ws);
    }
}

__attribute__((target("thumb")))
__attribute__((section(".text.SoftKeys_DrawString_Hook")))
void SoftKeys_DrawString_Hook(WSHDR *ws,
    int x, int y, int x2, int y2,
    int type, int text_flags,
    const char *pen, const char *brush) {
    int font = FONT_MEDIUM;
    if (*settings_p) {
        if (settings->soft_keys.enable) {
            font = settings->soft_keys.font;
            y += settings->soft_keys.v_offset;
            if (type == 2) { // icon
                if (!settings->soft_keys.show_middle_icon) {
                    return;
                }
            } else {
                y2 += settings->soft_keys.v_offset;
            }
            TransformText(ws, settings->soft_keys.text_transform);
        }
    }
    _DrawString(ws, x, y, x2, y2, font, text_flags, pen, brush);
}

__attribute__((section(".text.SoftKeys_DrawString_Trampoline")))
void SoftKeys_DrawString_Trampoline(WSHDR *ws,
    int x, int y, int x2, int y2,
    int font, int text_flags,
    const char *pen , const char *brush) {
    SoftKeys_DrawString_Hook(ws, x, y, x2, y2, font, text_flags, pen, brush);
}

#define DrawIMGHDR_BW ((void (*)(const IMGHDR *, int, int, const char *, const char *))(ADDR_DrawIMGHDR_BW))

__attribute__((target("thumb")))
__attribute__((section(".text.SoftKeys_DrawMiddleIcon_Hook")))
void SoftKeys_DrawMiddleIcon_Hook(const IMGHDR *img, int x, int y, const char *pen, const char *brush) {
    if (*settings_p) {
        if (settings->soft_keys.enable) {
            if (!settings->soft_keys.show_middle_icon) {
                return;
            }
        }
    }
    DrawIMGHDR_BW(img, x, y, pen, brush);
}
