#include <swilib.h>
#include "ipc.h"

const char *IPC_NAME = "89ThemeEngine";

void IPC_SendMessage(int submess, IPC_REQ *ipc_req, IPC_DATA *data) {
    ipc_req->name_to = IPC_NAME;
    ipc_req->name_from = IPC_NAME;
    ipc_req->data = data;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, submess, ipc_req);
}

void IPC_DestroyMessage(IPC_REQ *ipc_req) {
    if (ipc_req) {
        if (ipc_req->data) {
            mfree(ipc_req->data);
        }
        mfree(ipc_req);
    }
}
