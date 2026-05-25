#include <swilib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbar.h"
#include "config.h"
#include "registry.h"
#include "theme_utils.h"

#define THEME_CACHE_DIR "1:\\ThemeCache"
#define COLOR_CACHE_FILE_NAME "ColorControls.col"

const char *GetFileName(enum ThemeCacheImageID id) {
    switch (id) {
        case TCI_HEADLINE_DEFAULT:
            return "background_picture_headline_default";
        case TCI_HEADLINE_FULLSCREEN:
            return "background_picture_headline_fullscreen";
        case TCI_BODY_DEFAULT:
            return "background_picture_body_default";
        case TCI_BODY_TAB:
            return "background_picture_body_tab";
        case TCI_BOTTOM_DEFAULT:
            return "background_picture_bottom_default";
        case TCI_BOTTOM_FULLSCREEN:
            return "background_picture_bottom_fullscreen";
        case TCI_POPUP_OPTIONS:
            return "background_picture_options_popup";
        case TCI_POPUP_FEEDBACK:
            return "background_picture_feedback_popup";
        case TCI_SELECTION_1_LINE:
            return "background_picture_selection_list";
        case TCI_SELECTION_2_LINE:
            return "background_picture_selection_2lines";
        case TCI_SELECTION_3_LINE:
            return "background_picture_selection_3lines";
        case TCI_SELECTION_ICON_ONLY:
            return "background_picture_selection_icon";
        case TCI_POPUP_SEARCH_FIELD:
            return "background_picture_searchfield_popup";
        case TCI_POPUP_QUICK_ACCESS_FIELD:
            return "background_picture_quickaccessfield_popup";
        case TCI_PROGRESS_BAR:
            return "progressbar";
        case TCI_TAB_SELECTED:
            return "tabulator_background_selected";
        case TCI_TAB_UNSELECTED:
            return "tabulator_background_unselected";
        case TCI_STATUS_BAR_STANDARD:
            return "statusbar_standard";
        case TCI_STATUS_BAR_FULLSCREEN:
            return "statusbar_fullscreen";
        default:
            return NULL;
    }
}

void GetThemeImageCachePath(char *dest, enum ThemeCacheImageID id) {
    sprintf(dest, "%s\\%s.dat", THEME_CACHE_DIR, GetFileName(id));
}

void GetPITCachePath(char *dest, int icon) {
    sprintf(dest, "%s\\%d.dat", THEME_CACHE_DIR, icon);
}

static int SaveImageCache(const IMGHDR *img, const char *path) {
    if (img) {
        FILE *file = fopen(path, "wb");
        if (file) {
            const uint16_t w = img->w;
            const uint16_t h = img->h;
            const uint8_t bpnum = (uint8_t)img->bpnum;
            const size_t size = CalcBitmapSize((short)w, (short)h, bpnum);
            if (fwrite(&w, sizeof(uint16_t), 1, file) != 1) {
                goto ERROR;
            }
            if (fwrite(&h, sizeof(uint16_t), 1, file) != 1) {
                goto ERROR;
            }
            if (fwrite(&bpnum, sizeof(uint8_t), 1, file) != 1) {
                goto ERROR;
            }
            if (!img->bitmap) {
                goto ERROR;
            }
            if (fwrite(img->bitmap, size, 1, file) != 1) {
                ERROR:
                fclose(file);
                uint32_t err;
                sys_unlink(path, &err);
                return 0;
            }
            fclose(file);
            return 1;
        }
    }
    return 0;
}

static int SaveColorCache() {
    int success = 0;
    char buffer[1536];
    char *p = buffer;
    p += sprintf(p, "%s\n%s\n", "ColourScheme 89ThemeEngine", "Data");
    for (int i = 0; i < 60; i++) {
        const char *col = GetPaletteAdrByColorIndex(100 + i);
        p += sprintf(p, "%03d %03d %03d %03d\n", col[0], col[1], col[2], col[3]);
    }
    char path[128];
    sprintf(path, "%s\\%s", THEME_CACHE_DIR, COLOR_CACHE_FILE_NAME);
    FILE *file = fopen(path, "w");
    if (file) {
        const size_t size = p - buffer;
        success = (fwrite(buffer, size, 1, file) == 1) ? 1 : 0;
        fclose(file);
        if (success) {
            WSHDR ws;
            uint16_t wsbody[128];
            CreateLocalWS(&ws, wsbody, 127);
            str_2ws(&ws, path, 127);
            success = (Registry_SetResourcePath(REGISTRY_HMI_ID_COLOR_CONTROLS, 1, &ws) == 0) ? 1 : 0;
        }
    }
    return success;
}

IMGHDR *LoadPITCache(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file) {
        uint16_t w;
        uint16_t h;
        uint8_t bpnum;
        if (fread(&w, sizeof(uint16_t), 1, file) != 1) {
            goto ERROR;
        }
        if (fread(&h, sizeof(uint16_t), 1, file) != 1) {
            goto ERROR;
        }
        if (fread(&bpnum, sizeof(uint8_t), 1, file) != 1) {
            goto ERROR;
        }
        const int size = CalcBitmapSize((short)w, (short)h, bpnum);
        uint8_t *bitmap = malloc(size);
        if (!bitmap) {
            goto ERROR;
        }
        if (fread(bitmap, size, 1, file) != 1) {
            mfree(bitmap);
            ERROR:
            fclose(file);
            return NULL;
        }
        fclose(file);

        IMGHDR *img = malloc(sizeof(IMGHDR));
        if (img) {
            img->w = w;
            img->h = h;
            img->bpnum = bpnum;
            img->bitmap = bitmap;
        } else {
            mfree(bitmap);
        }
        return img;
    }
    return NULL;
}

int ThemeCache_Save() {
    char path[128];
    uint32_t err = 0;

    const int r = isdir(THEME_CACHE_DIR, &err);
    if (r != 1) { // !dir
        if (!sys_mkdir(THEME_CACHE_DIR, &err)) {
            return 0;
        }
    }

    IMGHDR *img = NULL;
    for (int theme_cache_id = 0; theme_cache_id < TCI_TOTAL; theme_cache_id++) {
        GetThemeImageCachePath(path, theme_cache_id);
        if (theme_cache_id == TCI_SELECTION_ICON_ONLY) {
            sys_unlink(path, &err);
            PBar_Step(PBAR_TEXT_DELETED, ThemeUtils_GetThemeImageDisplayName(theme_cache_id));
        } else {
            img = GetIMGHDRFromThemeCache(theme_cache_id);
            if (SaveImageCache(img, path)) {
                PBar_Step(PBAR_TEXT_APPLIED, ThemeUtils_GetThemeImageDisplayName(theme_cache_id));
            } else {
                return 0;
            }
        }
    }

    if (SaveColorCache()) {
        PBar_Step(PBAR_TEXT_SAVED, COLOR_CACHE_FILE_NAME);
    } else {
        return 0;
    }

    const uint16_t pit[1] = {CFG.pit.separator};
    for (int i = 0; i < 1; i++) {
        const int icon = pit[i];
        GetPITCachePath(path, icon);
        img = GetPITaddr(icon);
        if (SaveImageCache(img, path)) {
            char *text = ThemeUtils_GetPITImageDisplayName(icon);
            PBar_Step(PBAR_TEXT_SAVED, text);
            mfree(text);
        } else {
            return 0;
        }
    }
    return 1;
}

int ThemeCache_LoadPIT() {
    char path[128];
    const uint16_t pit[1] = {CFG.pit.separator};
    for (int i = 0; i < 1; i++) {
        GetPITCachePath(path, pit[i]);
        IMGHDR *img = LoadPITCache(path);
        if (!img) {
            return 0;
        }
        PIT_ResetImage(pit[i]);
        PIT_SetImage(pit[i], img);
    }
    return 1;
}
