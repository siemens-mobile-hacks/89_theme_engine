#include <swilib.h>
#include "ipc.h"

const char *IPC_NAME = "89ThemeEngine";

void IPC_SendMessage(int submess, IPC_DATA *data) {
    static IPC_REQ ipc;
    IPC_REQ *p = &ipc;
    p->name_to = IPC_NAME;
    p->name_from = IPC_NAME;
    p->data = data;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, submess, p);
}
