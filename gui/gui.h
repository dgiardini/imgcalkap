#ifndef __GUI_IMGCALKAP_INC_
#define __GUI_IMGCALKAP_INC
#include "../imgcalkap.h"
extern int wxShowMsg(char * title, char * msg, char type);
extern int wxShowCoordDlg(data_dialog_t * data);
extern int wxShowMapDlg(data_dialog_t * data);
extern int wxShowOpenFileWindowWin(void);
extern int wxShowSaveWindowWin(void);
#endif