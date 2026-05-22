#include <swilib.h>
#include "registry.h"

int Registry_RegClient_Wallpaper() {
    REG_CLIENT client = { 0 };
    client.cepid = MMI_CEPID;
    client.msg_id = REGISTRY_CLIENT_MSG_ID;
    client.hmi_keys = (short[]){REGISTRY_HMI_ID_WALLPAPER};
    client.hmi_keys_count = 1;
    return Registry_RegClient(&client);
}
