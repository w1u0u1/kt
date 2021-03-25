#ifndef _FILE_
#define _FIlE_

#include "header.h"

#define FILE_PATH_MAX_NUM 1024


void file_close(IN HANDLE FileHandle);
HANDLE file_create(IN PWSTR FileName, IN ULONG DesiredAccess, IN ULONG ShareAccess, IN ULONG CreateDisposition);
BOOL file_delete(PWSTR FileName);
BOOL file_get_size(IN HANDLE FileHandle, OUT PLARGE_INTEGER pFileSize);
BOOL file_read(IN HANDLE FileHandle, OUT PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset);
BOOL file_write(IN HANDLE FileHandle, IN PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset);
BOOL file_unlock(PCSTR FileName);
NTSTATUS file_irp_delete(PCSTR FileName);


#endif //_FILE_