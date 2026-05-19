#pragma once

enum {
    IPC_RUN,
    IPC_APPLY_THEME,
};

typedef struct {
    void *data0;
    void *data1;
    void *data2;
} IPC_DATA;

void IPC_SendMessage(int submess, IPC_DATA *data);

extern const char *IPC_NAME;
