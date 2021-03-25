#ifndef _PROCESS_
#define _PROCESS_

#include "header.h"


BOOL process_hide(HANDLE hHideProcessId);
BOOL process_hide2(PUCHAR pszHideProcessName);
BOOL process_kill(HANDLE hProcessId);
BOOL process_kill2(PUCHAR pszKillProcessName);


#endif //_PROCESS_