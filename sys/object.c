#include "object.h"
#include "mem.h"
#include "sysinfo.h"
#include "module.h"


NTSTATUS object_callback_remove(PVOID RegistrationHandle)
{
	ObUnRegisterCallbacks(RegistrationHandle);
	return STATUS_SUCCESS;
}

VOID object_callback_enum(_In_ POBJECT_TYPE ObjectType, _In_ ULONG_PTR Base, _In_ ULONG Size, _Out_ ULONG_PTR* pPreCallbackAddress, _Out_ ULONG_PTR* pPostCallbackAddress)
{
	int offset = 0;

	switch (sysinfo_os_version())
	{
	case WIN_VERSION_2K:
	case WIN_VERSION_XP:
	case WIN_VERSION_2K3:
	case WIN_VERSION_VISTA_2008:
		break;
	case WIN_VERSION_WIN7_2008R2:
		offset = 0xC0;
		break;
	case WIN_VERSION_WIN8_2012:
	case WIN_VERSION_WIN81_2012R2:
	case WIN_VERSION_WIN10:
		offset = 0xC8;
		break;
	}

	if (offset == 0)
		return;

	POBJECT_CALLBACK_ENTRY pCallbackEntry = *(POBJECT_CALLBACK_ENTRY*)((UCHAR*)ObjectType + offset);
	POBJECT_CALLBACK_ENTRY Head = pCallbackEntry;

	while ((POBJECT_CALLBACK_ENTRY)pCallbackEntry->CallbackList.Flink != Head)
	{
		if (MmIsAddressValid(pCallbackEntry->CallbackEntry->Altitude.Buffer))
		{
			ULONG_PTR PreOp = (ULONG_PTR)pCallbackEntry->PreOperation;
			ULONG_PTR PostOp = (ULONG_PTR)pCallbackEntry->PostOperation;

			if (PreOp > Base && PreOp < (Base + Size))
				*pPreCallbackAddress = &pCallbackEntry->PreOperation;
			if (PostOp > Base && PostOp < (Base + Size))
				*pPostCallbackAddress = &pCallbackEntry->PostOperation;
		}

		pCallbackEntry = (POBJECT_CALLBACK_ENTRY)pCallbackEntry->CallbackList.Flink;
	}
}

POB_CALLBACK_ADDRESSES object_callback_enum_process_thread(const char* ModuleName)
{
	ULONG_PTR moduleBase = 0;
	ULONG moduleSize = 0;
	POB_CALLBACK_ADDRESSES pAddrs = NULL;

	if (!module_check_name(ModuleName, &moduleBase, &moduleSize))
		return NULL;

	pAddrs = mem_alloc(sizeof(OB_CALLBACK_ADDRESSES));
	if (pAddrs == NULL)
		return NULL;

	object_callback_enum(*PsProcessType, moduleBase, moduleSize, &pAddrs->pProcPreCallback, &pAddrs->pProcPostCallback);
	object_callback_enum(*PsThreadType, moduleBase, moduleSize, &pAddrs->pThreadPreCallback, &pAddrs->pThreadPostCallback);

	if (pAddrs->pProcPreCallback)
		pAddrs->OrigProcPre = *pAddrs->pProcPreCallback;

	if (pAddrs->pThreadPreCallback)
		pAddrs->OrigThreadPre = *pAddrs->pThreadPreCallback;

	if (pAddrs->pProcPostCallback)
		pAddrs->OrigProcPost = *pAddrs->pProcPostCallback;

	if (pAddrs->pThreadPostCallback)
		pAddrs->OrigThreadPost = *pAddrs->pThreadPostCallback;

	return pAddrs;
}

OB_PREOP_CALLBACK_STATUS object_callback_disable_PreCallback()
{
	return OB_PREOP_SUCCESS;
}

VOID object_callback_disable_PostCallback()
{
	return;
}

VOID object_callback_disable_process_thread(POB_CALLBACK_ADDRESSES pAddrs)
{
	if (pAddrs->pProcPreCallback)
		*pAddrs->pProcPreCallback = object_callback_disable_PreCallback;

	if (pAddrs->pThreadPreCallback)
		*pAddrs->pThreadPreCallback = object_callback_disable_PreCallback;

	if (pAddrs->pProcPostCallback)
		*pAddrs->pProcPostCallback = object_callback_disable_PostCallback;

	if (pAddrs->pThreadPostCallback)
		*pAddrs->pThreadPostCallback = object_callback_disable_PostCallback;
}

VOID object_callback_enable_process_thread(POB_CALLBACK_ADDRESSES pAddrs)
{
	if (pAddrs->pProcPreCallback)
		*pAddrs->pProcPreCallback = pAddrs->OrigProcPre;

	if (pAddrs->pThreadPreCallback)
		*pAddrs->pThreadPreCallback = pAddrs->OrigThreadPre;

	if (pAddrs->pProcPostCallback)
		*pAddrs->pProcPostCallback = pAddrs->OrigProcPost;

	if (pAddrs->pThreadPostCallback)
		*pAddrs->pThreadPostCallback = pAddrs->OrigThreadPost;
}