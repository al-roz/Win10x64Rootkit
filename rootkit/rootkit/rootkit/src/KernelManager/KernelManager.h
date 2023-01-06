#pragma once
#include "../stdafx.h"
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

    ULONG GetSectionByAddres(ULONG_PTR addres);

    ULONG_PTR GetZeroMemmoryInSections(ULONG sections);
};