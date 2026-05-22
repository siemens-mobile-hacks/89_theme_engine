#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <swilib.h>
#include "skin.h"
#include "pbar.h"
#include "config.h"
#include "img_utils.h"
#include "theme_utils.h"

#define BODY_TAB_SEPARATOR_Y 33
#define POPUP_OPTIONS_CLEAR_H 5
#define POPUP_OPTIONS_SEPARATOR_Y 34
#define POPUP_FEEDBACK_CROP_Y  40
#define POPUP_FEEDBACK_CLEAR_W 4
#define POPUP_FEEDBACK_CLEAR_H 28
#define POPUP_FIELD_CLEAR_H 1
#define POPUP_SEARCH_FIELD_CLEAR_W 33

#define Cs_SetColor(id, rgba) RscMgr_CsSetColor(id - 100, rgba)
#define Cs_SetTransparent(id) RscMgr_CsSetColor(id - 100, (uint8_t[4]){0x00, 0x00, 0x00, 0x00})
#define Cs_Update() RscMgr_CsUpdate()

static uint16_t InjectImage(IMGHDR *dest, const IMGHDR *wallpaper, int crop_y) {
    uint8_t *bitmap = (dest) ? dest->bitmap : NULL;
    if (bitmap) {
        IMGHDR *new_img = IMGHDR_CropVertical(wallpaper, crop_y, dest->h);
        if (new_img) {
            memcpy(dest, new_img, sizeof(IMGHDR));
            mfree(bitmap);
            mfree(new_img);
            return dest->h;
        }
    }
    return 0;
}

static int InjectImageSolid(IMGHDR *dest, const uint8_t rgba[4]) {
    uint8_t *bitmap = (dest) ? dest->bitmap : NULL;
    if (bitmap) {
        IMGHDR *new_img = IMGHDR_CreateSolid(dest->w, dest->h, rgba);
        if (new_img) {
            memcpy(dest, new_img, sizeof(IMGHDR));
            mfree(bitmap);
            mfree(new_img);
            return dest->h;
        }
    }
    return 0;
}

static int ConvertImage(IMGHDR *image) {
    IMGHDR *img = IMGHDR_ARGB8888_to_RGB565(image);
    if (img) {
        mfree(image->bitmap);
        memcpy(image, img, sizeof(IMGHDR));
        mfree(img);
        return 1;
    }
    return 0;
}

static IMGHDR *PrepareBgImage(IMGHDR *dest, const IMGHDR *wallpaper, uint16_t crop_y,
                            int blur, int blur_radius, int overlay, const uint8_t *overlay_col) {
    if (!dest || !dest->bitmap) {
        return NULL;
    }
    uint16_t w = 0, h = 0;
    h = InjectImage(dest, wallpaper, crop_y);
    if (!h) {
        return NULL;
    }
    w = dest->w;
    if (blur) {
        IMGHDR_BoxBlur(dest, blur_radius);
    }
    if (overlay) {
        IMGHDR *img_overlay = IMGHDR_CreateSolid(w, h, overlay_col);
        if (img_overlay) {
            IMGHDR_Blend(dest, img_overlay);
            FreeIMGHDR(img_overlay);
        }
    }
    return dest;
}

static int ApplyGeneral() {
    PIT_ResetImage(CFG.separator_icon_id);
    const IMGHDR *separator = GetPITaddr(CFG.separator_icon_id);
    if (separator) {
        IMGHDR *new_separator = IMGHDR_CreateSolid(separator->w, separator->h, (uint8_t [4]){0x00, 0x00, 0x00, 0x00});
        if (!new_separator) {
            return CFG.separator_icon_id;
        }
        IMGHDR_DrawHLine(new_separator, 0, 0, new_separator->w - 1, LINE_DOTTED, SKIN.general.separator_col);
        PIT_SetImage(CFG.separator_icon_id, new_separator);
    }
    PBar_Step(PBAR_TEXT_APPLY, "Menu separator");
    Cs_SetColor(TPC_SELECT_BORDER, SKIN.general.select_border_col);
    Cs_SetColor(TPC_SEPARATOR, SKIN.general.separator_col);
    return 0;
}

static int ApplyStatusBar(const IMGHDR *wallpaper, uint16_t *crop_y) {
    int theme_cache_id = TCI_STATUS_BAR_STANDARD;
    IMGHDR *status_bar_standard = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, *crop_y,
        SKIN.status_bar.blur, SKIN.status_bar.blur_radius,
        SKIN.status_bar.overlay, SKIN.status_bar.overlay_col);
    if (!status_bar_standard) {
        return theme_cache_id;
    }
    if (!ConvertImage(status_bar_standard)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_STATUS_BAR_FULLSCREEN;
    IMGHDR *status_bar_fullscreen = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!status_bar_fullscreen) {
        return theme_cache_id;
    }
    if (!IMGHDR_Clone(status_bar_fullscreen, status_bar_standard)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    *crop_y += status_bar_standard->h;
    return 0;
}

static int ApplyHeadline(const IMGHDR *wallpaper, uint16_t *crop_y) {
    int theme_cache_id = TCI_HEADLINE_DEFAULT;
    IMGHDR *headline_default = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, *crop_y,
        SKIN.headline.blur, SKIN.headline.blur_radius,
        SKIN.headline.overlay, SKIN.headline.overlay_col);
    if (!headline_default) {
        return theme_cache_id;
    }
    if (SKIN.headline.border) {
        const int border_w = (int)SKIN.headline.border_width - 1;
        const int border_x = (ScreenW() - border_w) / 2;
        const int border_x2 = border_x + border_w;
        if (SKIN.headline.border == SKIN_HEADLINE_BORDER_BOTTOM) {
            IMGHDR_DrawHLine(headline_default, border_x, headline_default->h - 1, border_x2, 0, SKIN.headline.border_col);
        } else if (SKIN.headline.border == SKIN_HEADLINE_BORDER_TOP_BOTTOM) {
            IMGHDR_DrawHLine(headline_default, border_x, 0, border_x2, 0, SKIN.headline.border_col);
            IMGHDR_DrawHLine(headline_default, border_x, headline_default->h - 1, border_x2, 0, SKIN.headline.border_col);
        }
    }
    if (!ConvertImage(headline_default)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_HEADLINE_FULLSCREEN;
    IMGHDR *headline_fullscreen = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!headline_fullscreen) {
        return theme_cache_id;
    }
    if (!IMGHDR_Clone(headline_fullscreen, headline_default)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    Cs_SetColor(TPC_HEADER_FOREGROUND, SKIN.headline.text_col);
    Cs_SetColor(TPC_EXTRA_HEADER_FOREGROUND, SKIN.headline.text_col);
    Cs_SetColor(TPC_LIGHT_TEXT_FOREGROUND, SKIN.headline.text_col);
    Cs_SetTransparent(TPC_LIGHT_TEXT_BACKGROUND);
    Cs_SetColor(TPC_WINDOW_HEADER_FOREGROUND, SKIN.headline.text_col);
    *crop_y += headline_fullscreen->h;
    return 0;
}

static int ApplyBody(const IMGHDR *wallpaper, uint16_t *crop_y) {
    int theme_cache_id = TCI_BODY_DEFAULT;
    IMGHDR *body_default = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, *crop_y,
        SKIN.body.blur, SKIN.body.blur_radius,
        SKIN.body.overlay, SKIN.body.overlay_col);
    if (!body_default) {
        return theme_cache_id;
    }
    if (!ConvertImage(body_default)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_BODY_TAB;
    IMGHDR *body_tab = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, *crop_y,
        SKIN.body.blur, SKIN.body.blur_radius,
        SKIN.body.overlay, SKIN.body.overlay_col);
    if (!body_tab) {
        return theme_cache_id;
    }
    if (SKIN.tabs.separator) {
        const int separator_w = (int)SKIN.tabs.separator_width - 1;
        const int separator_x = (ScreenW() - separator_w) / 2;
        const int separator_x2 = separator_x + separator_w;
        IMGHDR_DrawHLine(body_tab, separator_x, BODY_TAB_SEPARATOR_Y, separator_x2, 0, SKIN.tabs.separator_col);
    }
    if (!ConvertImage(body_tab)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    Cs_SetColor(TPC_FOREGROUND, SKIN.body.main_text_col);
    Cs_SetColor(TPC_DISABLED_TEXT_FOREGROUND, SKIN.body.disabled_text_col);
    Cs_SetColor(TPC_EDIT_HEADER_FOREGROUND, SKIN.body.edit_header_text_col);
    Cs_SetColor(TPC_EDIT_FOREGROUND, SKIN.body.edit_text_col);
    *crop_y += body_default->h;
    return 0;
}

static int ApplyBottom(const IMGHDR *wallpaper, const uint16_t *crop_y) {
    int theme_cache_id = TCI_BOTTOM_DEFAULT;
    IMGHDR *bottom_default = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, *crop_y,
        SKIN.bottom.blur, SKIN.bottom.blur_radius,
        SKIN.bottom.overlay, SKIN.bottom.overlay_col);
    if (!bottom_default) {
        return theme_cache_id;
    }
    if (SKIN.bottom.border) {
        IMGHDR_DrawHLine(bottom_default, 0, 0, bottom_default->w - 1, 0, SKIN.bottom.border_col);
    }
    if (!ConvertImage(bottom_default)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_BOTTOM_FULLSCREEN;
    if (!IMGHDR_Clone(GetIMGHDRFromThemeCache(theme_cache_id), bottom_default)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    Cs_SetColor(TPC_BOTTOM_FOREGROUND, SKIN.bottom.text_col);
    return 0;
}

static int ApplySelection() {
    for (int theme_cache_id = TCI_SELECTION_1_LINE; theme_cache_id <= TCI_SELECTION_3_LINE; theme_cache_id++) {
        IMGHDR *selection = GetIMGHDRFromThemeCache(theme_cache_id);
        if (!selection) {
            return theme_cache_id;
        }
        if (!InjectImageSolid(selection, SKIN.selection.bg_col)) {
            return theme_cache_id;
        }
        const int selection_clear_top_end = 1;
        // const int selection_clear_bottom_start = selection->h - 1 - 1;
        // const int selection_clear_bottom_end = selection->h - 1;
        IMGHDR_TransparentV(selection, 0, selection_clear_top_end);
        // IMGHDR_TransparentV(selection, selection_clear_bottom_start, selection_clear_bottom_end);
        if (SKIN.selection.border) {
            IMGHDR_DrawBorder(selection, 0, selection_clear_top_end, selection->w - 1, selection->h - 1, 0, SKIN.selection.border_col);
        }
        PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    }
    Cs_SetColor(TPC_SELECT_FOREGROUND, SKIN.selection.selected_text_col);
    Cs_SetColor(TPC_UNSELECT_FOREGOUND, SKIN.selection.unselected_text_col);
    Cs_SetColor(TPC_POPUP_SELECT_FOREGROUD, SKIN.selection.selected_text_col);
    Cs_SetColor(TPC_WINDOW_FOREGROUND, SKIN.selection.unselected_text_col);
    Cs_SetTransparent(TPC_POPUP_SELECT_BACKGROUD);
    return 0;
}

static int ApplyTabs() {
    int theme_cache_id = TCI_TAB_UNSELECTED;
    IMGHDR *tab_unselected = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!tab_unselected) {
        return theme_cache_id;
    }
    if (!InjectImageSolid(tab_unselected, (uint8_t [4]){0x00, 0x00, 0x00, 0x00})) {
        return theme_cache_id;
    }
    if (!ConvertImage(tab_unselected)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_TAB_SELECTED;
    IMGHDR *tab_selected = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!tab_selected) {
        return theme_cache_id;
    }
    IMGHDR *tab_selected_mask = IMGHDR_CreateGlowMask(tab_selected->w, tab_selected->h, SKIN.tabs.selected_bg_col);
    if (!tab_selected_mask) {
        return theme_cache_id;
    }
    mfree(tab_selected->bitmap);
    memcpy(tab_selected, tab_selected_mask, sizeof(IMGHDR));
    mfree(tab_selected_mask);
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    return 0;
}

static int ApplyPopups(const IMGHDR* wallpaper) {
    int crop_y = POPUP_FEEDBACK_CROP_Y;
    int theme_cache_id = TCI_POPUP_FEEDBACK;
    // Feedback
    IMGHDR* popup_feedback = PrepareBgImage(GetIMGHDRFromThemeCache(theme_cache_id), wallpaper, crop_y,
                                            SKIN.popup.blur, SKIN.popup.blur_radius,
                                            SKIN.popup.overlay, SKIN.popup.overlay_col);
    if (!popup_feedback) {
        return theme_cache_id;
    }
    const int popup_feedback_clear_top_end = POPUP_FEEDBACK_CLEAR_H;
    const int popup_feedback_clear_bottom_start = popup_feedback->h - 1 - POPUP_FEEDBACK_CLEAR_H;
    const int popup_feedback_clear_left_end = POPUP_FEEDBACK_CLEAR_W;
    const int popup_feedback_clear_right_start = popup_feedback->w - 1 - POPUP_FEEDBACK_CLEAR_W;
    IMGHDR_TransparentV(popup_feedback, 0, popup_feedback_clear_top_end);
    IMGHDR_TransparentV(popup_feedback, popup_feedback_clear_bottom_start, popup_feedback->h - 1);
    IMGHDR_TransparentH(popup_feedback, 0, popup_feedback_clear_left_end);
    IMGHDR_TransparentH(popup_feedback, popup_feedback_clear_right_start, popup_feedback->w - 1);
    if (SKIN.popup.border) {
        IMGHDR_DrawBorder(popup_feedback,
                          popup_feedback_clear_left_end, popup_feedback_clear_top_end,
                          popup_feedback_clear_right_start, popup_feedback_clear_bottom_start,
                          0, SKIN.popup.border_col
        );
    }
    if (!ConvertImage(popup_feedback)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    // Options
    theme_cache_id = TCI_POPUP_OPTIONS;
    IMGHDR *status_bar = GetIMGHDRFromThemeCache(TCI_STATUS_BAR_STANDARD);
    IMGHDR *headline = GetIMGHDRFromThemeCache(TCI_HEADLINE_DEFAULT);
    if (!headline || !status_bar) {
        return theme_cache_id;
    }
    crop_y = status_bar->h + headline->h;
    IMGHDR* popup_options = GetIMGHDRFromThemeCache(theme_cache_id);
    popup_options->h = popup_feedback->h + 1;
    popup_options = PrepareBgImage(popup_options, wallpaper, crop_y,
                                   SKIN.popup.blur, SKIN.popup.blur_radius,
                                   SKIN.popup.overlay, SKIN.popup.overlay_col);
    if (!popup_options) {
        return theme_cache_id;
    }
    const int popup_options_clear_top_end = POPUP_OPTIONS_CLEAR_H;
    IMGHDR_TransparentV(popup_options, 0, popup_options_clear_top_end);
    if (SKIN.popup.border) {
        IMGHDR_DrawBorder(popup_options,
                          0, popup_options_clear_top_end,
                          popup_options->w - 1, popup_options->h - 1,
                          0, SKIN.popup.border_col);
    }
    if (SKIN.popup.options.header_separator) {
        const int separator_w = (int)SKIN.popup.options.header_separator_width - 1;
        const int separator_x = (ScreenW() - separator_w) / 2;
        const int separator_x2 = separator_x + separator_w;
        IMGHDR_DrawHLine(popup_options, separator_x, POPUP_OPTIONS_SEPARATOR_Y, separator_x2, 0, SKIN.popup.border_col);
    }
    if (!ConvertImage(popup_options)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    // Fields
    theme_cache_id = TCI_POPUP_SEARCH_FIELD;
    IMGHDR *popup_search_field = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!popup_search_field) {
        return theme_cache_id;
    }
    if (!InjectImageSolid(popup_search_field, SKIN.popup.fields.bg_col)) {
        return theme_cache_id;
    }
    const int popup_field_clear_bottom_start = popup_search_field->h - 1 - POPUP_FIELD_CLEAR_H;
    const int popup_field_clear_bottom_end = popup_search_field->h - 1;
    IMGHDR_TransparentV(popup_search_field, popup_field_clear_bottom_start, popup_field_clear_bottom_end);
    if (SKIN.popup.fields.border) {
        IMGHDR_DrawBorder(popup_search_field,
            0, 0, popup_search_field->w - 1, popup_field_clear_bottom_start,
            0, SKIN.popup.fields.border_col);
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    theme_cache_id = TCI_POPUP_QUICK_ACCESS_FIELD;
    IMGHDR *popup_quick_access_field = GetIMGHDRFromThemeCache(theme_cache_id);
    if (!popup_quick_access_field) {
        return theme_cache_id;
    }
    if (!IMGHDR_Clone(popup_quick_access_field, popup_search_field)) {
        return theme_cache_id;
    }
    const int popup_search_field_clear_top_end = 1;
    const int popup_search_field_clear_left_end = POPUP_SEARCH_FIELD_CLEAR_W;
    IMGHDR_TransparentV(popup_search_field, 0, popup_search_field_clear_top_end);
    IMGHDR_TransparentH(popup_search_field, 0, popup_search_field_clear_left_end);
    if (SKIN.popup.fields.border) {
        IMGHDR_DrawHLine(popup_search_field,
            popup_search_field_clear_left_end, popup_search_field_clear_top_end, popup_search_field->w - 1,
            0, SKIN.popup.fields.border_col);
        IMGHDR_DrawVLine(popup_search_field,
            popup_search_field_clear_left_end, popup_search_field_clear_top_end, popup_field_clear_bottom_start,
            0, SKIN.popup.fields.border_col);
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    return 0;
}

static int ApplyScrollBar() {
    Cs_SetColor(TPC_SCROLL_BAR, SKIN.scroll_bar.bg_col);
    Cs_SetColor(TPC_SCROLL_BAR_SLIDER, SKIN.scroll_bar.slider_col);
    return 0;
}

static int ApplyProgressBar() {
    const int theme_cache_id = TCI_PROGRESS_BAR;
    if (!InjectImageSolid(GetIMGHDRFromThemeCache(theme_cache_id), SKIN.progress_bar.fg_col)) {
        return theme_cache_id;
    }
    PBar_Step(PBAR_TEXT_APPLY, ThemeUtils_GetImageDisplayName(theme_cache_id));
    Cs_SetColor(TPC_PROGRESS_BAR_BACKGROUND, SKIN.progress_bar.bg_col);
    return 0;
}

static int ApplyCalendar() {
    Cs_SetColor(TPC_CALENDAR_DAY_BACKGROUND, SKIN.calendar.day_bg_col);
    Cs_SetColor(TPC_CALENDAR_WEEKEND_BACKGROUND, SKIN.calendar.weekend_bg_col);
    Cs_SetColor(TPC_CALENDAR_EVENT_BACKGROUND, SKIN.calendar.event_bg_col);
    Cs_SetColor(TPC_CALENDAR_FOREGROUND, SKIN.calendar.fg_col);
    return 0;
}

int Theme_Apply() {
    IMGHDR *wallpaper = GetCanvasBufferPicPtr(0);
    if (wallpaper) {
        wallpaper->bpnum = IMGHDR_TYPE_RGB565;
        if (wallpaper->w != ScreenW() && wallpaper->h != ScreenH()) {
            MsgBoxError(0x11, (int)"Wrong wallpaper size");
            return -1;
        }
        int error = 0;
        uint16_t crop_y = 0;
        error = ApplyGeneral();
        if (error) {
            return error;
        }
        error = ApplyStatusBar(wallpaper, &crop_y);
        if (error) {
            return error;
        }
        error = ApplyHeadline(wallpaper, &crop_y);
        if (error) {
            return error;
        }
        error = ApplyBody(wallpaper, &crop_y);
        if (error) {
            return error;
        }
        error = ApplyBottom(wallpaper, &crop_y);
        if (error) {
            return error;
        }
        error = ApplySelection();
        if (error) {
            return error;
        }
        error = ApplyTabs();
        if (error) {
            return error;
        }
        error = ApplyPopups(wallpaper);
        if (error) {
            return error;
        }
        error = ApplyScrollBar();
        if (error) {
            return error;
        }
        error = ApplyProgressBar();
        if (error) {
            return error;
        }
        error = ApplyCalendar();
        if (error) {
            return error;
        }
        Cs_Update();
        return 0;
    }
    return -1;
}
