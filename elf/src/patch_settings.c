#include <swilib.h>
#include <stdlib.h>
#include "config.h"
#include "patch_settings.h"

SETTINGS *settings;

void PatchSettings_Init() {
    SETTINGS **settings_p = (SETTINGS**)strtoul(CFG.patch.settings_ram_addr, NULL, 16);
    if (settings_p && !*settings_p) {
        *settings_p = malloc(sizeof(SETTINGS));
        settings = *settings_p;
        PatchSettings_Apply();
    }
}

void PatchSettings_Apply() {
    if (settings) {
        zeromem(settings, sizeof(SETTINGS));
        settings->soft_keys.enable = CFG.patch.soft_keys.enable;
        settings->soft_keys.font = CFG.patch.soft_keys.font;
        settings->soft_keys.v_offset = CFG.patch.soft_keys.v_offset;
        settings->soft_keys.text_transform = CFG.patch.soft_keys.text_transform;
        settings->soft_keys.show_middle_icon = CFG.patch.soft_keys.show_middle_icon;
    }
}

void PatchSettings_Destroy() {
    SETTINGS **settings_p = (SETTINGS**)strtoul(CFG.patch.settings_ram_addr, NULL, 16);
    if (settings_p && settings) {
        mfree(settings);
        *settings_p = NULL;
    }
}
