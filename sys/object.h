#ifndef _OBJECT_
#define _OBJECT_

#include "header.h"


typedef struct _OB_CALLBACK_ADDRESSES
{
	ULONG_PTR* pProcPreCallback, * pProcPostCallback;
	ULONG_PTR* pThreadPreCallback, * pThreadPostCallback;
	ULONG_PTR OrigProcPre, OrigProcPost;
	ULONG_PTR OrigThreadPre, OrigThreadPost;
}OB_CALLBACK_ADDRESSES, * POB_CALLBACK_ADDRESSES;


POB_CALLBACK_ADDRESSES object_callback_enum_process_thread(const char* ModuleName);
VOID object_callback_disable_process_thread(POB_CALLBACK_ADDRESSES pAddrs);
VOID object_callback_enable_process_thread(POB_CALLBACK_ADDRESSES pAddrs);



#endif //_OBJECT_