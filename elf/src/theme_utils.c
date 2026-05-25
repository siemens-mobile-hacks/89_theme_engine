#include <stdio.h>
#include <stdlib.h>
#include <swilib.h>

const char *ThemeUtils_GetThemeImageDisplayName(enum ThemeCacheImageID id) {
    switch (id) {
        case TCI_HEADLINE_DEFAULT:
            return "Headline (default)";
        case TCI_HEADLINE_FULLSCREEN:
            return "Headline (fullscreen)";
        case TCI_BODY_DEFAULT:
            return "Body (default)";
        case TCI_BODY_TAB:
            return "Body (tab)";
        case TCI_BOTTOM_DEFAULT:
            return "Bottom (default)";
        case TCI_BOTTOM_FULLSCREEN:
            return "Bottom (fullscreen)";
        case TCI_POPUP_OPTIONS:
            return "Popup options";
        case TCI_POPUP_FEEDBACK:
            return "Popup feedback";
        case TCI_SELECTION_1_LINE:
            return "Selection (1 line)";
        case TCI_SELECTION_2_LINE:
            return "Selection (2 lines)";
        case TCI_SELECTION_3_LINE:
            return "Selection (3 lines)";
        case TCI_SELECTION_ICON_ONLY:
            return "Selection (icon only)";
        case TCI_POPUP_SEARCH_FIELD:
            return "Popup search field";
        case TCI_POPUP_QUICK_ACCESS_FIELD:
            return "Popup quick access field";
        case TCI_PROGRESS_BAR:
            return "Progress bar";
        case TCI_TAB_SELECTED:
            return "Tab (selected)";
        case TCI_TAB_UNSELECTED:
            return "Tab (unselected)";
        case TCI_STATUS_BAR_STANDARD:
            return "Status bar (standard)";
        case TCI_STATUS_BAR_FULLSCREEN:
            return "Status bar (fullscreen)";
        default: return "Unknown";
    }
}

char *ThemeUtils_GetPITImageDisplayName(int icon) {
    char *name = malloc(32);
    snprintf(name, 32, "%d.png", icon);
    return name;
}
