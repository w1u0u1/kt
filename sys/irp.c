#include "irp.h"
#include "sysinfo.h"


NTSTATUS IoCompletionRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	*Irp->UserIosb = Irp->IoStatus;

	if (Irp->UserEvent)
		KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, 0);

	if (Irp->MdlAddress)
	{
		IoFreeMdl(Irp->MdlAddress);
		Irp->MdlAddress = NULL;
	}

	IoFreeIrp(Irp);
	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS irp_delete_file(HANDLE FileHandle)
{
	NTSTATUS          status;
	PFILE_OBJECT      pFileObject;
	PDEVICE_OBJECT    pDeviceObject;
	PIRP              pIrp;
	KEVENT            kevent;
	FILE_DISPOSITION_INFORMATION    fileDispositionInformation;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;
	PSECTION_OBJECT_POINTERS pSectionObjectPointer;

	status = ObReferenceObjectByHandle(FileHandle, 1, *IoFileObjectType, KernelMode, &pFileObject, NULL);
	if (!NT_SUCCESS(status))
		return STATUS_UNSUCCESSFUL;

	pDeviceObject = IoGetRelatedDeviceObject(pFileObject);

	pIrp = IoAllocateIrp(pDeviceObject->StackSize, TRUE);
	if (pIrp == NULL)
	{
		ObDereferenceObject(pFileObject);
		return STATUS_UNSUCCESSFUL;
	}

	KeInitializeEvent(&kevent, SynchronizationEvent, FALSE);

	RtlZeroMemory(&fileDispositionInformation, 0, sizeof(FILE_DISPOSITION_INFORMATION));

	fileDispositionInformation.DeleteFile = TRUE;

	pIrp->AssociatedIrp.SystemBuffer = &fileDispositionInformation;
	pIrp->UserEvent = &kevent;
	pIrp->UserIosb = &ioStatus;
	pIrp->Tail.Overlay.OriginalFileObject = pFileObject;
	pIrp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	pIrp->RequestorMode = KernelMode;

	irpSp = IoGetNextIrpStackLocation(pIrp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = pDeviceObject;
	irpSp->FileObject = pFileObject;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.FileObject = pFileObject;

	IoSetCompletionRoutine(pIrp, IoCompletionRoutine, &kevent, TRUE, TRUE, TRUE);

	pSectionObjectPointer = pFileObject->SectionObjectPointer;
	pSectionObjectPointer->ImageSectionObject = 0;
	pSectionObjectPointer->DataSectionObject = 0;

	IoCallDriver(pDeviceObject, pIrp);

	KeWaitForSingleObject(&kevent, Executive, KernelMode, TRUE, NULL);

	ObDereferenceObject(pFileObject);

	return STATUS_SUCCESS;
}