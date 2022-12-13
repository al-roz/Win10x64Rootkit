#include "LoadedPe.h"

BOOLEAN LoadedPe::InitializeFields(PWCHAR fileNameWC)
{
	DbgBreakPoint();
	UNICODE_STRING fileNameUS;
	OBJECT_ATTRIBUTES ObjectAttributes;

	RtlInitUnicodeString(&fileNameUS, fileNameWC);

	error = STATUS_SUCCESS;

	InitializeObjectAttributes(&ObjectAttributes, &fileNameUS,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
		error = STATUS_UNSUCCESSFUL;
		return FALSE;
	}

	HANDLE fileHandle;
	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS status = ZwCreateFile(&fileHandle,
		GENERIC_READ,
		&ObjectAttributes,
		&ioStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);

	if (!NT_SUCCESS(status))
	{
		error = status;
		return FALSE;
	}	
		
	FILE_STANDARD_INFORMATION standardInformation = { 0 };

	status = ZwQueryInformationFile(fileHandle, &ioStatusBlock, &standardInformation, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);

	if (!NT_SUCCESS(status))
	{
		error = status;
		return FALSE;
	}

	
	auto fileSize = standardInformation.EndOfFile.LowPart;
	this->loadedBuffer = (unsigned char*)ExAllocatePool(PagedPool, fileSize);
	if (!this->loadedBuffer)
	{
		ZwClose(fileHandle);
		return FALSE;
	}

	LARGE_INTEGER ByteOffset;
	ByteOffset.LowPart = ByteOffset.HighPart = 0;
			
	status = ZwReadFile(fileHandle,
		NULL, NULL, NULL,
		&ioStatusBlock,
		this->loadedBuffer,
		fileSize,
		&ByteOffset, NULL);
	

	ZwClose(fileHandle);
	return TRUE;
}

unsigned char* LoadedPe::GetLoadedBuffer()
{
	return this->loadedBuffer;
}

PeData LoadedPe::GetPeData()
{
	return this->data;
}

NTSTATUS LoadedPe::GetError()
{
	return this->error;
}

void LoadedPe::Free()
{
	ExFreePool(this->loadedBuffer);
	this->loadedBuffer = NULL;
}

