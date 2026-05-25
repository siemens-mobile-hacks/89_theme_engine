#include <swilib.h>
#include <stdlib.h>
#include "config.h"
#include "patch_settings.h"

SETTINGS *settings;

void PatchSettings_Init() {
    SETTINGS **settings_p = (SETTINGS**)strtoul(CFG.settings_ram_addr, NULL, 16);
    if (settings_p) {
        if (*settings_p) {
            mfree(*settings_p);
            *settings_p = NULL;
        }
        *settings_p = malloc(sizeof(SETTINGS));
        settings = *settings_p;
        if (settings) {
            zeromem(settings, sizeof(SETTINGS));
            settings->soft_keys.enable = CFG.soft_keys.enable;
            settings->soft_keys.font = CFG.soft_keys.font;
            settings->soft_keys.v_offset = CFG.soft_keys.v_offset;
            settings->soft_keys.text_transform = CFG.soft_keys.text_transform;
            settings->soft_keys.show_middle_icon = CFG.soft_keys.show_middle_icon;
            return;
        }
    }
    MsgBoxError(0x11, (int)"Error apply patch settings");
}

