#pragma once

#include "../settings.h"

#define ADDR_DrawString     (0xA094A3AB)
#define ADDR_wstrtoupper    (0xA04FA78B)
#define ADDR_wstrtolower    (0xA04FA84D)
#define ADDR_wstrcapitalize (0xA04FA929)

#define ADDR_DrawIMGHDR_BW  (0xA0A0CB0A | 1)

#define settings_p ((SETTINGS**)(0xA8E3736C))
#define settings (*settings_p)
