#include <swilib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipc.h"
#include "skin.h"
#include "pbar.h"
#include "theme.h"
#include "config.h"
#include "patch_settings.h"
#include "registry.h"
#include "theme_cache.h"

typedef struct {
    int gui_id;
    int current;
    int total;
    WSHDR *text;
} PBAR;

typedef struct {
    CSM_RAM csm;
    int reg_client_id;
    char skin_path[128];
    int is_applying_theme;
    int popup_gui_id;
    PBAR pbar;
    GBSTMR tmr_apply_theme;
} MAIN_CSM;

const int minus11 =- 11;
unsigned short maincsm_name_body[140];

void InitPBar(MAIN_CSM *csm) {
    zeromem(&csm->pbar, sizeof(PBAR));
    csm->pbar.total = TCI_TOTAL + 1; // total images + separator
    csm->pbar.total += TCI_TOTAL + 2; // cache: total images + col + separator
}

void ApplyTheme(MAIN_CSM *csm) {
    if (Skin_Load(csm->skin_path) == 0) {
        csm->is_applying_theme = 1;
        const int err = Theme_Apply();
        if (err == 0) {
            if (!ThemeCache_Save()) {
                ShowMSG(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Theme applied, but cache not saved");
            } else {
                ShowMSG(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Theme has been applied");
            }
            strcpy(CFG.skin_path, csm->skin_path);
            if (!Config_Save()) {
                MsgBoxError(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Failed to save config");
            }
        } else {
            char msg[32];
            sprintf(msg, "Failed to apply theme, error %d", err);
            ShowMSG(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)msg);
        }
        GeneralFunc_flag1(csm->pbar.gui_id, 1);
        InitPBar(csm);
        csm->is_applying_theme = 0;
    } else {
        MsgBoxError(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Failed to load skin");
    }
}

void ApplyThemeTimerProc(GBSTMR *tmr) {
    ApplyTheme_IPC();
}

void ApplyThemeTimer(MAIN_CSM *csm) {
    GBS_StartTimerProc(&csm->tmr_apply_theme, MsToTicks(CFG.apply_delay_ms), ApplyThemeTimerProc);
}

void ApplyThemeYesNo(const int no) {
    if (!no) {
        ApplyTheme_IPC();
    }
}

void LoadPIT() {
    if (!ThemeCache_LoadPIT()) {
        MsgBoxError(DIALOG_NORMAL | DIALOG_FULLSCREEN, (int)"Failed to load PIT cache");
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
        IPC_REQ *ipc_req = msg->data0;
        if (strcmpi(ipc_req->name_to, IPC_NAME) == 0) {
            IPC_DATA *ipc_data = ipc_req->data;
            if (msg->submess == IPC_RUN) {
                const int csm_id = (int)ipc_data->data0;
                const char *exe_path = ipc_data->data1;
                const char *file_path = ipc_data->data2;
                if (csm->csm.id != csm_id) {
                    CloseCSM(csm_id);
                    if (*file_path) {
                        strcpy(csm->skin_path, file_path);
                        if (!csm->popup_gui_id) {
                            csm->popup_gui_id = MsgBoxYesNo(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Apply theme?", ApplyThemeYesNo);
                        }
                    } else {
                        char path[128];
                        if (GetDefaultSkinPath(path, exe_path)) {
                            Skin_Save(path);
                        }
                    }
                    return 0;
                }
                if (csm->reg_client_id == - 1) {
                    csm->reg_client_id = Registry_RegClient_Wallpaper();
                    if (csm->reg_client_id < 0) {
                        MsgBoxError(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Failed to register client with Registry");
                        csm->reg_client_id = -1;
                    }
                }
                if (!*file_path) {
                    SUBPROC(LoadPIT);
                }
            } else if (msg->submess == IPC_APPLY_THEME) {
                if (!csm->is_applying_theme) {
                    csm->pbar.gui_id = PBar_Create();
                    SUBPROC(ApplyTheme, csm);
                } else {
                    MsgBoxError(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"Theme is already being applied");
                }
                IPC_DestroyMessage(ipc_req);
            } else if (msg->submess == IPC_PBAR_STEP) {
                if (csm->pbar.gui_id) {
                    csm->pbar.current++;
                    if (!csm->pbar.text) {
                        csm->pbar.text = AllocWS(64);
                        SetPBarText(csm->pbar.gui_id, csm->pbar.text);
                    }
                    wsprintf(csm->pbar.text, "%t", ipc_data->data0);
                    const float percent = (float)(csm->pbar.current) / (float)csm->pbar.total;
                    SetPBarValue(csm->pbar.gui_id, (int)(100 * percent));
                }
                mfree(ipc_data->data0);
                IPC_DestroyMessage(ipc_req);
            }
        }
    } else if (msg->msg == MSG_GUI_DESTROYED) {
        const int gui_id = (int)msg->data0;
        if (gui_id == csm->popup_gui_id) {
            csm->popup_gui_id = 0;
        } else if (gui_id == csm->pbar.gui_id) {
            csm->pbar.gui_id = 0;
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(msg->data0, CFG_PATH) == 0) {
            Config_Init();
            strcpy(csm->skin_path, CFG.skin_path);
            PatchSettings_Apply();
            ShowMSG(DIALOG_NORMAL | DIALOG_DUMMY_CSM, (int)"89ThemeEngine config updated!");
        } else if (strcmpi(msg->data0, csm->skin_path) == 0) {
            ApplyTheme_IPC();
        }
    } else if (msg->msg == REGISTRY_CLIENT_MSG_ID) {
        strcpy(csm->skin_path, CFG.skin_path);
        ApplyThemeTimer(csm);
    }
    return 1;
}

void OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    csm->reg_client_id = -1;
    strcpy(csm->skin_path, CFG.skin_path);
    InitPBar(csm);
    PatchSettings_Init();
}

void OnClose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    PatchSettings_Destroy();
    if (csm->reg_client_id != -1) {
        Registry_UnregClient(csm->reg_client_id);
    }
    GBS_DelTimer(&(csm->tmr_apply_theme));
    GeneralFunc_flag1(csm->popup_gui_id, 1);
    GeneralFunc_flag1(csm->pbar.gui_id, 1);
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

    static IPC_REQ ipc_req;
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
    IPC_SendMessage(IPC_RUN, &ipc_req, &data);

    return 0;
}

#pragma GCC diagnostic pop
