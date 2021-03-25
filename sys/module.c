#include "module.h"
#include "mem.h"


BOOL module_check_name(char* ModuleName, ULONG_PTR* ModuleBase, ULONG* ModuleSize)
{
	NTSTATUS status;
	ULONG	ulSize = 0, i = 0;
	PSYSTEM_MODULE_LIST pModuleList = NULL;

	status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &ulSize);
	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return FALSE;

	pModuleList = (PSYSTEM_MODULE_LIST)mem_alloc(ulSize);
	if (pModuleList == NULL)
		return FALSE;

	status = ZwQuerySystemInformation(SystemModuleInformation, pModuleList, ulSize, &ulSize);
	if (!NT_SUCCESS(status))
	{
		mem_free(pModuleList);
		return FALSE;
	}

	for (i = 0; i < pModuleList->ulCount; i++)
	{
		if (_stricmp(pModuleList->smi[i].ImageName + pModuleList->smi[i].ModuleNameOffset, ModuleName) == 0)
		{
			if (ModuleBase != NULL)
				*ModuleBase = pModuleList->smi[i].Base;

			if (ModuleSize != NULL)
				*ModuleSize = pModuleList->smi[i].Size;

			mem_free(pModuleList);
			return TRUE;
		}
	}

	mem_free(pModuleList);
	return FALSE;
}