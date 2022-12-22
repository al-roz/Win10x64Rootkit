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