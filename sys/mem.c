#include "mem.h"

#define TAG 'MEM'


PVOID mem_alloc(SIZE_T NumberOfBytes)
{
	PVOID p = ExAllocatePoolWithTag(NonPagedPool, NumberOfBytes, TAG);
	if (p == NULL)
		return NULL;

	RtlZeroMemory(p, NumberOfBytes);
	return p;
}

VOID mem_free(PVOID p)
{
	if(p)
		ExFreePoolWithTag(p, TAG);
}