#pragma once

enum {
    IPC_RUN,
    IPC_APPLY_THEME,
    IPC_PBAR_STEP,
};

typedef struct {
    void *data0;
    void *data1;
    void *data2;
} IPC_DATA;

void IPC_SendMessage(int submess, IPC_REQ *ipc_req, IPC_DATA *data);
void IPC_DestroyMessage(IPC_REQ *ipc_req);

extern const char *IPC_NAME;
