#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include "pbar.h"
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

static int WriteImageCache(enum ThemeCacheImageID id) {
    char path[128];
    const IMGHDR *img = GetIMGHDRFromThemeCache(id);
    if (img) {
        const char *file_name = GetFileName(id);
        if (!file_name) {
            return 0;
        }
        sprintf(path, "%s\\%s.dat", THEME_CACHE_DIR, file_name);
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

static int WriteColorCache() {
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

int ThemeCache_Save() {
    uint32_t err = 0;
    const int r = isdir(THEME_CACHE_DIR, &err);
    if (r != 1) { // !dir
        if (r == 0) { // file
            return 0;
        }
        if (sys_mkdir(THEME_CACHE_DIR, &err) != 0) {
            return 0;
        }
    }
    for (int theme_cache_id = 0; theme_cache_id < TCI_TOTAL; theme_cache_id++) {
        if (theme_cache_id == TCI_SELECTION_ICON_ONLY) {
            continue;
        }
        if (WriteImageCache(theme_cache_id) != 1) {
            return 0;
        }
        PBar_Step(PBAR_TEXT_SAVE, ThemeUtils_GetImageDisplayName(theme_cache_id));
    }
    if (WriteColorCache() != 1) {
        return 0;
    }
    PBar_Step(PBAR_TEXT_SAVE, COLOR_CACHE_FILE_NAME);
    return 1;
}
