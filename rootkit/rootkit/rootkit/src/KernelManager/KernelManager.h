#pragma once
#include <ntddk.h>
#include <aux_klib.h>
#include <basetsd.h>
#include <windef.h>

#include "../Utils/Utils.h"
#include "../Structs/Structs.h"


class KernelManager
{
private:  

    KernelManager() = default;
    KernelManager(const KernelManager&) = delete;    

    PeData kernelData;

public:    

    static KernelManager& getInstance()
    {
        static KernelManager instance;
        return instance;
    }

    BOOLEAN InitializeInstanceData();

    PeData GetKernelData();

    void PrintExport();

    void PrintSections();

    ExportData FindFunctionFromExportByName(PCHAR funcName);

    ExportData FindFunctionFromExportByAddres(ULONG_PTR funcAddr);

};