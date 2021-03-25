#include "file.h"
#include "irp.h"


void file_close(IN HANDLE FileHandle)
{
	ZwClose(FileHandle);
}

HANDLE file_create(IN PWSTR FileName, IN ULONG DesiredAccess, IN ULONG ShareAccess, IN ULONG CreateDisposition)
{
	HANDLE					hFile = NULL;
	NTSTATUS				status;
	IO_STATUS_BLOCK			iostatus = { 0 };
	OBJECT_ATTRIBUTES		objectAttributes = { 0 };
	UNICODE_STRING			ustrFileName;

	RtlInitUnicodeString(&ustrFileName, FileName);
	InitializeObjectAttributes(&objectAttributes, &ustrFileName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwCreateFile(&hFile, DesiredAccess, &objectAttributes, &iostatus, NULL, FILE_ATTRIBUTE_NORMAL,
		ShareAccess, CreateDisposition, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if (!NT_SUCCESS(status))
		return NULL;

	return hFile;
}

BOOL file_delete(PWSTR FileName)
{
	NTSTATUS status;
	IO_STATUS_BLOCK	iostatus;
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	UNICODE_STRING ustrFilePath;

	RtlInitUnicodeString(&ustrFilePath, FileName);
	InitializeObjectAttributes(&ObjectAttributes, &ustrFilePath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwDeleteFile(&ObjectAttributes);
	if (!NT_SUCCESS(status))
		return FALSE;

	return TRUE;
}

BOOL file_get_size(IN HANDLE FileHandle, OUT PLARGE_INTEGER pFileSize)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatus = { 0 };
	FILE_STANDARD_INFORMATION FileStandard = { 0 };

	status = ZwQueryInformationFile(FileHandle, &ioStatus, &FileStandard, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(status))
		return FALSE;

	*pFileSize = FileStandard.EndOfFile;
	return TRUE;
}

BOOL file_read(IN HANDLE FileHandle, OUT PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset)
{
	NTSTATUS status;
	IO_STATUS_BLOCK	iostatus;

	status = ZwReadFile(FileHandle, NULL, NULL, NULL, &iostatus, Buffer, Length, ByteOffset, NULL);
	if (!NT_SUCCESS(status))
		return FALSE;

	return TRUE;
}

BOOL file_write(IN HANDLE FileHandle, IN PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset)
{
	NTSTATUS status;
	IO_STATUS_BLOCK iostatus;

	status = ZwWriteFile(FileHandle, NULL, NULL, NULL, &iostatus, Buffer, Length, ByteOffset, NULL);
	if (!NT_SUCCESS(status))
		return FALSE;

	return TRUE;
}

BOOL file_unlock_check(SYSTEM_HANDLE_TABLE_ENTRY_INFO sysHandleTableEntryInfo, UNICODE_STRING ustrUnlockFileName)
{
	NTSTATUS status = STATUS_SUCCESS;
	CLIENT_ID clientId = { 0 };
	OBJECT_ATTRIBUTES objectAttributes = { 0 };
	HANDLE hSourceProcess = NULL;
	HANDLE hDupObj = NULL;
	POBJECT_NAME_INFORMATION pObjNameInfo = NULL;
	ULONG ulObjNameInfoSize = 0;
	BOOL bRet = FALSE;
	WCHAR wszSrcFile[FILE_PATH_MAX_NUM] = { 0 };
	WCHAR wszDestFile[FILE_PATH_MAX_NUM] = { 0 };

	do
	{
		InitializeObjectAttributes(&objectAttributes, NULL, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		RtlZeroMemory(&clientId, sizeof(clientId));
		clientId.UniqueProcess = sysHandleTableEntryInfo.UniqueProcessId;

		status = ZwOpenProcess(&hSourceProcess, PROCESS_ALL_ACCESS, &objectAttributes, &clientId);
		if (!NT_SUCCESS(status))
			break;

		status = ZwDuplicateObject(hSourceProcess, sysHandleTableEntryInfo.HandleValue, NtCurrentProcess(), &hDupObj, PROCESS_ALL_ACCESS, 0, DUPLICATE_SAME_ACCESS);
		if (!NT_SUCCESS(status))
			break;

		ZwQueryObject(hDupObj, 1, NULL, 0, &ulObjNameInfoSize);
		if (0 >= ulObjNameInfoSize)
			break;

		pObjNameInfo = ExAllocatePool(NonPagedPool, ulObjNameInfoSize);
		if (NULL == pObjNameInfo)
			break;

		RtlZeroMemory(pObjNameInfo, ulObjNameInfoSize);

		status = ZwQueryObject(hDupObj, 1, pObjNameInfo, ulObjNameInfoSize, &ulObjNameInfoSize);
		if (!NT_SUCCESS(status))
			break;

		RtlZeroMemory(wszSrcFile, FILE_PATH_MAX_NUM * sizeof(WCHAR));
		RtlZeroMemory(wszDestFile, FILE_PATH_MAX_NUM * sizeof(WCHAR));
		RtlCopyMemory(wszSrcFile, pObjNameInfo->Name.Buffer, pObjNameInfo->Name.Length);
		RtlCopyMemory(wszDestFile, ustrUnlockFileName.Buffer, ustrUnlockFileName.Length);

		if (wcslen(wszSrcFile) > 0)
		{
			if (wcsstr(wszSrcFile, wszDestFile))
			{
				//DbgPrint("[File]%wZ\n", &pObjNameInfo->Name);
				bRet = TRUE;
				break;
			}
		}
	} while (FALSE);

	if (NULL != pObjNameInfo)
		ExFreePool(pObjNameInfo);
	if (NULL != hDupObj)
		ZwClose(hDupObj);
	if (NULL != hSourceProcess)
		ZwClose(hSourceProcess);

	return bRet;
}

BOOL file_unlock_close(SYSTEM_HANDLE_TABLE_ENTRY_INFO sysHandleTableEntryInfo)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS pEProcess = NULL;
	HANDLE hFileHandle = NULL;
	KAPC_STATE apcState = { 0 };
	OBJECT_HANDLE_FLAG_INFORMATION objectHandleFlagInfo = { 0 };

	hFileHandle = sysHandleTableEntryInfo.HandleValue;

	status = PsLookupProcessByProcessId(sysHandleTableEntryInfo.UniqueProcessId, &pEProcess);
	if (!NT_SUCCESS(status))
		return FALSE;

	KeStackAttachProcess(pEProcess, &apcState);

	objectHandleFlagInfo.Inherit = 0;
	objectHandleFlagInfo.ProtectFromClose = 0;
	ObSetHandleAttributes((HANDLE)hFileHandle, &objectHandleFlagInfo, KernelMode);

	ZwClose((HANDLE)hFileHandle);

	KeUnstackDetachProcess(&apcState);
	return TRUE;
}

BOOL file_unlock(PCSTR FileName)
{
	NTSTATUS status = STATUS_SUCCESS;
	SYSTEM_HANDLE_INFORMATION  tempSysHandleInfo = { 0 };
	PSYSTEM_HANDLE_INFORMATION  pSysHandleInfo = NULL;
	ULONG ulSysHandleInfoSize = 0;
	ULONG ulReturnLength = 0;
	ULONGLONG ullSysHandleCount = 0;
	PSYSTEM_HANDLE_TABLE_ENTRY_INFO pSysHandleTableEntryInfo = NULL;
	ULONGLONG i = 0;
	BOOL bRet = FALSE;
	UNICODE_STRING ustrUnlockFileName;
	ANSI_STRING	astr;

	RtlInitAnsiString(&astr, FileName);
	RtlAnsiStringToUnicodeString(&ustrUnlockFileName, &astr, TRUE);

	ZwQuerySystemInformation(16, &tempSysHandleInfo, sizeof(tempSysHandleInfo), &ulSysHandleInfoSize);
	if (0 >= ulSysHandleInfoSize)
		return FALSE;

	pSysHandleInfo = (PSYSTEM_HANDLE_INFORMATION)ExAllocatePool(NonPagedPool, ulSysHandleInfoSize);
	if (NULL == pSysHandleInfo)
		return FALSE;

	RtlZeroMemory(pSysHandleInfo, ulSysHandleInfoSize);

	status = ZwQuerySystemInformation(16, pSysHandleInfo, ulSysHandleInfoSize, &ulReturnLength);
	if (!NT_SUCCESS(status))
	{
		ExFreePool(pSysHandleInfo);
		return FALSE;
	}

	ullSysHandleCount = pSysHandleInfo->NumberOfHandles;
	pSysHandleTableEntryInfo = pSysHandleInfo->Handles;

	for (i = 0; i < ullSysHandleCount; i++)
	{
		bRet = file_unlock_check(pSysHandleTableEntryInfo[i], ustrUnlockFileName);
		if (bRet)
			file_unlock_close(pSysHandleTableEntryInfo[i]);
	}

	ExFreePool(pSysHandleInfo);
}

NTSTATUS file_irp_delete(PCSTR FileName)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	HANDLE  hFile = NULL;
	ANSI_STRING astrFile;
	UNICODE_STRING ustrFile;

	RtlInitAnsiString(&astrFile, FileName);
	RtlAnsiStringToUnicodeString(&ustrFile, &astrFile, TRUE);

	hFile = file_create(ustrFile.Buffer, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE, FILE_OPEN);
	if (hFile == NULL)
		return status;

	status = irp_delete_file(hFile);

	ZwClose(hFile);

	return status;
}