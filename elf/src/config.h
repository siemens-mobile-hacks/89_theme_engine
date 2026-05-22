#pragma once
#include <cfg_items.h>

typedef struct {
    const CFG_HDR cfghdr_0;
    char skin_path[128];
    const CFG_HDR cfghdr_1;
    int apply_delay_ms;
    const CFG_HDR cfghdr_2;
    int separator_icon_id;
} CONFIG;

extern CONFIG CFG;
extern char CFG_PATH[];

void Config_Init();
int Config_Save();

