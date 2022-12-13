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
