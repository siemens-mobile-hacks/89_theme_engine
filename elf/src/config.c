#include <swilib.h>
#include "config.h"

#define TEXT_TRANSFORM {{"Off"}, {"Capitalize"}, {"Uppercase"}, {"Lowercase"}}

CONFIG CFG = {
    {CFG_STR_WIN1251, "Settings RAM addr", 8, 8},
    "A8E3736C",
    {CFG_STR_UTF8, "Skin path", 3, 127},
    "0:\\Skins\\Red.89",
    {CFG_UINT, "Skin apply delay (ms)", 500, 10000},
    1500,
    {
        {CFG_LEVEL, "PIT", 1, 0},
        {CFG_UINT, "Separator icon", 0, 2000},
        1335,
        {CFG_LEVEL, "", 0, 0}
    }, {
        {CFG_LEVEL, "Soft keys", 1, 0},
        {CFG_CHECKBOX, "Enable tweak", 0 ,1},
        1,
        {CFG_UINT, "Font", 0, 16},
        FONT_SMALL,
        {CFG_INT, "Vertical offset", 0, 10},
        2,
        {CFG_CBOX, "Text transform", 0, 4},
        TEXT_TRANSFORM_UPPERCASE,
        TEXT_TRANSFORM,
        {CFG_CHECKBOX, "Show middle icon", 0, 1},
        0,
        {CFG_LEVEL, "", 0, 0},
    }
};

char CFG_PATH[] = "?:\\zbin\\etc\\89ThemeEngine.bcfg";

void Config_Init() {
    CFG_PATH[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(CFG_PATH, &CFG, sizeof(CFG)) == -1) {
        BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CFG));
    }
}

int Config_Save() {
    return (BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CFG)) == 0);
}
