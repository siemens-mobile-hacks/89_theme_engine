#include <stdio.h>
#include <stdlib.h>
#include <swilib.h>
#include <string.h>
#include "ipc.h"
#include "pbar.h"

PBAR_DESC PBAR_D = {
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

void PBar_Step(PBarTextType type, const char *text) {
    IPC_REQ *ipc_req = malloc(sizeof(IPC_REQ));
    IPC_DATA *ipc_data = malloc(sizeof(IPC_DATA));
    // ReSharper disable once CppDFAMemoryLeak
    ipc_data->data0 = malloc(64);
    if (text) {
        char *prefix = NULL;
        if (type == PBAR_TEXT_APPLIED) {
            prefix = "Applied";
        } else if (type == PBAR_TEXT_SAVED) {
            prefix = "Saved";
        } else if (type == PBAR_TEXT_DELETED) {
            prefix = "Deleted";
        }
        if (prefix) {
            snprintf(ipc_data->data0, 64, "%s: %s", prefix, text);
        } else {
            snprintf(ipc_data->data0, 64, "%s", text);
        }
    } else {
        strcpy(ipc_data->data0, "Unknown");
    }
    IPC_SendMessage(IPC_PBAR_STEP, ipc_req, ipc_data);
}

int PBar_Create() {
    PBAR_D.proc3 = GetPBarProc3();
    return ShowPBarBox(0x11, (int)"Applying theme...", &PBAR_D, 0, NULL);
};
