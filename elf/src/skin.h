#pragma once

#include <swilib.h>
#include <cfg_items.h>

enum {
    SKIN_HEADLINE_BORDER_OFF,
    SKIN_HEADLINE_BORDER_BOTTOM,
    SKIN_HEADLINE_BORDER_TOP_BOTTOM,
    SKIN_HEADLINE_BORDER_COUNT,
};

#pragma pack(push, 1)

typedef struct {
    const CFG_HDR cfghdr_b;
    // Select border
    const CFG_HDR cfghdr_0;
    uint8_t select_border_col[4];
    // Separator color
    const CFG_HDR cfghdr_1;
    uint8_t separator_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_GENERAL;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Blur
    const CFG_HDR cfghdr_0;
    int blur;
    // Blur radius
    const CFG_HDR cfghdr_1;
    int blur_radius;
    // Overlay
    const CFG_HDR cfghdr_2;
    int overlay;
    // Overlay color
    const CFG_HDR cfghdr_3;
    uint8_t overlay_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_STATUS_BAR;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Blur
    const CFG_HDR cfghdr_0;
    int blur;
    // Blur radius
    const CFG_HDR cfghdr_1;
    int blur_radius;
    // Overlay
    const CFG_HDR cfghdr_2;
    int overlay;
    // Overlay color
    const CFG_HDR cfghdr_3;
    uint8_t overlay_col[4];
    // Border
    const CFG_HDR cfghdr_4;
    int border;
    const CFG_CBOX_ITEM cfgcbox_4[3];
    // Border width
    const CFG_HDR cfghdr_5;
    unsigned int border_width;
    // Border color
    const CFG_HDR cfghdr_6;
    uint8_t border_col[4];
    // Text color
    const CFG_HDR cfghdr_7;
    uint8_t text_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_HEADLINE;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Blur
    const CFG_HDR cfghdr_0;
    int blur;
    // Blur radius
    const CFG_HDR cfghdr_1;
    int blur_radius;
    // Overlay
    const CFG_HDR cfghdr_2;
    int overlay;
    // Overlay color
    const CFG_HDR cfghdr_3;
    uint8_t overlay_col[4];
    // Main text color
    const CFG_HDR cfghdr_4;
    uint8_t main_text_col[4];
    // Disabled text color
    const CFG_HDR cfghdr_5;
    uint8_t disabled_text_col[4];
    // Edit header text color
    const CFG_HDR cfghdr_6;
    uint8_t edit_header_text_col[4];
    // Edit text color
    const CFG_HDR cfghdr_7;
    uint8_t edit_text_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_BODY;

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    uint8_t selected_bg_col[4];
    // Separator
    const CFG_HDR cfghdr_1;
    int separator;
    // Separator width
    const CFG_HDR cfghdr_2;
    int separator_width;
    // Separator color
    const CFG_HDR cfghdr_3;
    uint8_t separator_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_TABS;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Blur
    const CFG_HDR cfghdr_0;
    int blur;
    // Blur radius
    const CFG_HDR cfghdr_1;
    int blur_radius;
    // Overlay
    const CFG_HDR cfghdr_2;
    int overlay;
    // Overlay color
    const CFG_HDR cfghdr_3;
    uint8_t overlay_col[4];
    // Border
    const CFG_HDR cfghdr_4;
    int border;
    // Border color
    const CFG_HDR cfghdr_5;
    uint8_t border_col[4];
    // Text color
    const CFG_HDR cfghdr_6;
    uint8_t text_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_BOTTOM;

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    // Background color
    uint8_t bg_col[4];
    // Border
    const CFG_HDR cfghdr_1;
    int border;
    // Border color
    const CFG_HDR cfghdr_2;
    uint8_t border_col[4];
    // Selected text color
    const CFG_HDR cfghdr_3;
    uint8_t selected_text_col[4];
    // Unselected text color
    const CFG_HDR cfghdr_4;
    uint8_t unselected_text_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_SELECTION;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Header separator
    const CFG_HDR cfghdr_0;
    int header_separator;
    // Header separator width
    const CFG_HDR cfghdr_1;
    unsigned int header_separator_width;
    const CFG_HDR cfghdr_e;
} SKIN_POPUP_OPTIONS;

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    // Background color
    uint8_t bg_col[4];
    const CFG_HDR cfghdr_1;
    // Border
    int border;
    const CFG_HDR cfghdr_2;
    // Border color
    uint8_t border_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_POPUP_FIELDS;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Blur
    const CFG_HDR cfghdr_0;
    int blur;
    // Blur radius
    const CFG_HDR cfghdr_1;
    int blur_radius;
    // Overlay
    const CFG_HDR cfghdr_2;
    int overlay;
    // Overlay color
    const CFG_HDR cfghdr_3;
    uint8_t overlay_col[4];
    // Border
    const CFG_HDR cfghdr_4;
    int border;
    // Border color
    const CFG_HDR cfghdr_5;
    uint8_t border_col[4];
    // Options
    SKIN_POPUP_OPTIONS options;
    // Field
    SKIN_POPUP_FIELDS fields;
    const CFG_HDR cfghdr_e;
} SKIN_POPUP;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Background color
    const CFG_HDR cfghdr_0;
    uint8_t bg_col[4];
    // Slider color
    const CFG_HDR cfghdr_1;
    uint8_t slider_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_SCROLL_BAR;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Background color
    const CFG_HDR cfghdr_0;
    uint8_t bg_col[4];
    // Foreground color
    const CFG_HDR cfghdr_1;
    uint8_t fg_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_PROGRESS_BAR;

typedef struct {
    const CFG_HDR cfghdr_b;
    // Day background color
    const CFG_HDR cfghdr_0;
    uint8_t day_bg_col[4];
    const CFG_HDR cfghdr_1;
    // Weekend background color
    uint8_t weekend_bg_col[4];
    const CFG_HDR cfghdr_2;
    // Event background color
    uint8_t event_bg_col[4];
    const CFG_HDR cfghdr_3;
    // Foreground color
    uint8_t fg_col[4];
    const CFG_HDR cfghdr_e;
} SKIN_CALENDAR;

typedef struct {
    SKIN_GENERAL general;
    SKIN_STATUS_BAR status_bar;
    SKIN_HEADLINE headline;
    SKIN_BODY body;
    SKIN_TABS tabs;
    SKIN_BOTTOM bottom;
    SKIN_SELECTION selection;
    SKIN_POPUP popup;
    SKIN_SCROLL_BAR scroll_bar;
    SKIN_PROGRESS_BAR progress_bar;
    SKIN_CALENDAR calendar;
} SKIN_T;

#pragma pack(pop)

extern SKIN_T SKIN;

int Skin_Save(const char *path);
int Skin_Load(const char *path);
