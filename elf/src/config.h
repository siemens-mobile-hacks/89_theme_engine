#pragma once
#include <cfg_items.h>
#include "patch_settings.h"

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    int separator;
    const CFG_HDR cfghdr_e;
} CONFIG_PIT;

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    int enable;
    const CFG_HDR cfghdr_1;
    int font;
    const CFG_HDR cfghdr_2;
    int v_offset;
    const CFG_HDR cfghdr_3;
    int text_transform;
    const CFG_CBOX_ITEM cfgcbox_3[TEXT_TRANSFORM_TOTAL];
    const CFG_HDR cfghdr_4;
    int show_middle_icon;
    const CFG_HDR cfghdr_e;
} CONFIG_PATCH_SOFT_KEYS;

typedef struct {
    const CFG_HDR cfghdr_b;
    const CFG_HDR cfghdr_0;
    char settings_ram_addr[9];
    CONFIG_PATCH_SOFT_KEYS soft_keys;
    const CFG_HDR cfghdr_e;
} CONFIG_PATCH;

typedef struct {
    const CFG_HDR cfghdr_0;
    char skin_path[128];
    const CFG_HDR cfghdr_1;
    int apply_delay_ms;
    CONFIG_PIT pit;
    CONFIG_PATCH patch;
} CONFIG;

extern CONFIG CFG;
extern char CFG_PATH[];

void Config_Init();
int Config_Save();

