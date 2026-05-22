#include <swilib.h>
#include "config.h"

CONFIG CFG = {
    {CFG_STR_UTF8, "Skin path", 3, 127},
    "0:\\Skins\\Red.89",
    {CFG_UINT, "Skin apply delay (ms)", 500, 10000},
    1500,
    {CFG_UINT, "Separator icon id", 0, 2000},
    1335,
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
