#include "LoadedPe.h"

BOOLEAN LoadedPe::InitializePeData()
{
	this->data.startAddres = reinterpret_cast<ULONG_PTR>(this->loadedBuffer);
	this->data.dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(this->data.startAddres);

	if (this->data.dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Error DOS signature");
		return FALSE;
	}

	this->data.ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(this->data.startAddres + this->data.dosHeader->e_lfanew);

	if (this->data.ntHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Error NT signature");
		return FALSE;
	}


	this->data.sectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
		reinterpret_cast<PBYTE>(&this->data.ntHeader->OptionalHeader) + this->data.ntHeader->FileHeader.SizeOfOptionalHeader);

	this->data.sectionsCount = this->data.ntHeader->FileHeader.NumberOfSections;

	if (this->data.ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
	{
		this->data.importDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
			this->data.startAddres + RvaToOffset(this->data.ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
	}

	if (this->data.ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
	{
		this->data.exportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
			this->data.startAddres +  RvaToOffset(this->data.ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));
	}
}

BOOLEAN LoadedPe::InitializeLoadedBuffer(PWCHAR fileNameWC)
{
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

ULONGLONG LoadedPe::RvaToOffset(ULONGLONG rva)
{
	for (DWORD i = 0; i < this->data.sectionsCount; ++i) {
		if ((rva >= this->data.sectionHeader[i].VirtualAddress) &&
			(rva <= (static_cast<ULONG_PTR>(this->data.sectionHeader[i].VirtualAddress) + this->data.sectionHeader[i].Misc.VirtualSize)))
			return rva - this->data.sectionHeader[i].VirtualAddress + this->data.sectionHeader[i].PointerToRawData;
	}

	return 0;
}

BOOLEAN LoadedPe::InitializeFields(PWCHAR fileNameWC)
{	
	if (!this->InitializeLoadedBuffer(fileNameWC))
	{
		return FALSE;
	}

	if (!this->InitializePeData())
	{
		return FALSE;
	}

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

ImportData LoadedPe::FindFunctionFromImportByName(PCHAR funcName)
{
	auto import = this->data.importDescriptor;

	while (!(import->FirstThunk == 0 &&
		import->Characteristics == 0 &&
		import->ForwarderChain == 0 &&
		import->Name == 0 &&
		import->OriginalFirstThunk == 0 &&
		import->TimeDateStamp == 0))
	{
		LONG_PTR* importNameTable;
		LONG_PTR* iat;

		break;
	}
	return ImportData{ NULL, NULL };
}

ExportData LoadedPe::FindFunctionFromExportByName(PCHAR funcName)
{
	auto exp = this->data.exportDirectory;

	auto functionsArray = (DWORD*)(this->data.startAddres + RvaToOffset(exp->AddressOfFunctions));

	auto namesArray = (DWORD*)(this->data.startAddres + RvaToOffset(exp->AddressOfNames));

	auto nameOrdinalsArray = (WORD*)(this->data.startAddres + RvaToOffset(exp->AddressOfNameOrdinals));
	
	for (ULONG i = 0; i < exp->NumberOfNames; ++i)
	{
		auto exportFuncName = reinterpret_cast<PCHAR>(this->data.startAddres + RvaToOffset(namesArray[i]));

		if (_stricmp(funcName, exportFuncName) == 0)
		{
			return ExportData{ nameOrdinalsArray[i] + exp->Base,
							   (this->data.startAddres + RvaToOffset(functionsArray[nameOrdinalsArray[i]])),
							   reinterpret_cast<PCHAR>(this->data.startAddres + namesArray[i]) };
		}
	}

	return ExportData{ NULL, NULL, NULL };
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

