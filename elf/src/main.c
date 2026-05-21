#include <swilib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipc.h"
#include "skin.h"
#include "theme.h"
#include "config.h"

#define REGISTRY_CLIENT_MSG_ID    0x9991
#define REGISTRY_HMI_ID_WALLPAPER 0x65

typedef struct {
    CSM_RAM csm;
    int reg_client_id;
    char skin_path[128];
    int popup_gui_id;
    int please_wait_gui_id;
    GBSTMR tmr_apply_theme;
} MAIN_CSM;

const int minus11 =- 11;
unsigned short maincsm_name_body[140];

int ShowWait() {
    return ShowWaitBox(0x11, (int)"Applying theme...");
}

void RegClient(MAIN_CSM *csm) {
    if (csm->reg_client_id == - 1) {
        REG_CLIENT client = { 0 };
        client.cepid = MMI_CEPID;
        client.msg_id = REGISTRY_CLIENT_MSG_ID;
        client.hmi_keys = (short[]){REGISTRY_HMI_ID_WALLPAPER};
        client.hmi_keys_count = 1;
        csm->reg_client_id = Registry_RegClient(&client);
        if (csm->reg_client_id < 0) {
            MsgBoxError(0x11, (int)"Failed to register client with Registry");
            csm->reg_client_id = -1;
        }
    }
}

void ApplyTheme(MAIN_CSM *csm) {
    if (Skin_Load(csm->skin_path) == 0) {
        const int err = Theme_Apply();
        if (err == 0) {
            RegClient(csm);
            ShowMSG(0x11, (int)"Theme has been applied");
        } else {
            char msg[32];
            sprintf(msg, "Failed to apply theme, error %d", err);
            ShowMSG(0x11, (int)msg);
        }
    } else {
        MsgBoxError(0x11, (int)"Error loading skin");
    }
    GeneralFunc_flag1(csm->please_wait_gui_id, 1);
    csm->please_wait_gui_id = 0;
}

void ApplyThemeTimerProc(GBSTMR *tmr) {
    IPC_SendMessage(IPC_APPLY_THEME, NULL);
}

void ApplyThemeTimer(MAIN_CSM *csm) {
    GBS_StartTimerProc(&csm->tmr_apply_theme, MsToTicks(CFG.apply_delay_ms), ApplyThemeTimerProc);
}

void ApplyThemeYesNo(const int no) {
    if (!no) {
        IPC_SendMessage(IPC_APPLY_THEME, NULL);
    }
}

int GetDefaultSkinPath(char *dest, const char *exe_path) {
    const char *p = strrchr(exe_path, '\\');
    if (p) {
        const size_t len = p - exe_path;
        strncpy(dest, exe_path, len);
        dest[len] = '\0';
        strcat(dest, "\\Viktor.89");
        return 1;
    }
    return 0;
}

int OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (msg->msg == MSG_IPC) {
        const IPC_REQ *ipc = msg->data0;
        if (strcmpi(ipc->name_to, IPC_NAME) == 0) {
            const IPC_DATA *ipc_data = ipc->data;
            if (msg->submess == IPC_RUN) {
                const int csm_id = (int)ipc_data->data0;
                const char *exe_path = ipc_data->data1;
                const char *file_path = ipc_data->data2;
                if (csm->csm.id != csm_id) {
                    CloseCSM(csm_id);
                    if (*file_path) {
                        strcpy(csm->skin_path, file_path);
                        if (!csm->popup_gui_id) {
                            csm->popup_gui_id = MsgBoxYesNo(0x11, (int)"Apply theme?", ApplyThemeYesNo);
                        }
                    } else {
                        char path[128];
                        if (GetDefaultSkinPath(path, exe_path)) {
                            Skin_Save(path);
                        }
                    }
                    return 0;
                }
                if (CFG.auto_apply && strlen(CFG.skin_path)) {
                    strcpy(csm->skin_path, CFG.skin_path);
                    ApplyThemeTimer(csm);
                }
            } else if (msg->submess == IPC_APPLY_THEME) {
                if (!csm->please_wait_gui_id) {
                    csm->please_wait_gui_id = ShowWait();
                    SUBPROC(ApplyTheme, csm);
                } else {
                    MsgBoxError(0x11, (int)"Theme is already being applied");
                }
            }
        }
    } else if (msg->msg == MSG_GUI_DESTROYED) {
        const int gui_id = (int)msg->data0;
        if (gui_id == csm->popup_gui_id) {
            csm->popup_gui_id = 0;
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(msg->data0, CFG_PATH) == 0) {
            Config_Init();
            ShowMSG(0x11, (int)"89ThemeEngine config updated!");
        } else if (strcmpi(msg->data0, csm->skin_path) == 0) {
            IPC_SendMessage(IPC_APPLY_THEME, NULL);
        }
    } else if (msg->msg == REGISTRY_CLIENT_MSG_ID) {
        ApplyThemeTimer(csm);
    }
    return 1;
}

void OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    csm->reg_client_id = -1;
    csm->csm.state = CSM_STATE_OPEN;
}

void OnClose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (csm->reg_client_id != -1) {
        Registry_UnregClient(csm->reg_client_id);
    }
    GBS_DelTimer(&(csm->tmr_apply_theme));
    GeneralFunc_flag1(csm->popup_gui_id, 1);
    GeneralFunc_flag1(csm->please_wait_gui_id, 1);
    SUBPROC(kill_elf);
}

const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
        {
                OnMessage,
                OnCreate,
#ifdef NEWSGOLD
                0,
                0,
                0,
                0,
#endif
                OnClose,
                sizeof(MAIN_CSM),
                1,
                &minus11
        },
        {
                maincsm_name_body,
                NAMECSM_MAGIC1,
                NAMECSM_MAGIC2,
                0x0,
                139,
                0
        }
};

void UpdateCSMname(void) {
    wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "89ThemeEngine");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmain"

int main(const char *exe_path, const char *file_path) {
    Config_Init();
    UpdateCSMname();

    LockSched();
    const MAIN_CSM main_csm = { 0 };
    CSM_RAM *save_cmpc = CSM_root()->csm_q->current_msg_processing_csm;
    CSM_root()->csm_q->current_msg_processing_csm = CSM_root()->csm_q->csm.first;
    const int csm_id = CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
    CSM_root()->csm_q->current_msg_processing_csm = save_cmpc;
    UnlockSched();

    static char s_exe_path[128];
    static char s_file_path[128];
    static IPC_DATA data = { 0 };
    data.data0 = (void*)csm_id;
    strcpy(s_exe_path, exe_path);
    data.data1 = s_exe_path;
    if (file_path) {
        strcpy(s_file_path, file_path);
    } else {
        *s_file_path = 0;
    }
    data.data2 = s_file_path;
    IPC_SendMessage(IPC_RUN, &data);

    return 0;
}

#pragma GCC diagnostic pop
