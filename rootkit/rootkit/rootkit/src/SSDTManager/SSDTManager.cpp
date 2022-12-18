#include "SSDTManager.h"


BOOL SSDTManager::InitializeInstanceData()
{
    if (!this->ntdll.InitializeFields(L"\\SystemRoot\\system32\\ntdll.dll"))
    {
        return FALSE;        
    }

    auto indexNtCreateFile = this->GetIndexSyscallFromNtdll("NtCreateFile");

    ExportData realNtCreateFileFromExport = KernelManager::getInstance().FindFunctionFromExportByName("NtCreateFile");

    if (indexNtCreateFile == -1)
    {
        return FALSE;
    }    

    bool found = FALSE;
    ULONG kiServiceTableOffset = 0;
    PeData kernelData = KernelManager::getInstance().GetKernelData();
    ULONG_PTR kernelBase = kernelData.startAddres;
    SIZE_T kernelSize = kernelData.ImageSize;

    

    for (; kiServiceTableOffset < kernelSize; kiServiceTableOffset++)
    {
        PDWORD supposedKiServiceTable = reinterpret_cast<PDWORD>(kernelBase + kiServiceTableOffset);        

        ULONG_PTR NtCreateFileAddres = reinterpret_cast<ULONG_PTR>(supposedKiServiceTable + (*(supposedKiServiceTable + indexNtCreateFile) >> 4));        
        
        if ( realNtCreateFileFromExport.functionAddres == NtCreateFileAddres)
        {
            found = TRUE;
            this->KiServiceTable = supposedKiServiceTable;
            DbgBreakPoint();
            break;
        }        
    }

    if (!found)
    {
        return FALSE;
    }

    
    return TRUE;
}

DWORD SSDTManager::GetIndexSyscallFromNtdll(PCHAR funcName)
{
    auto exportData = this->ntdll.FindFunctionFromExportByName(funcName);    
    
    auto funcByteArray = reinterpret_cast<PBYTE>(exportData.functionAddres);

    for (int i = 0; i < 100; i++)
    {
        // ret
        if (funcByteArray[i] == 0xC2 || funcByteArray[i] == 0xC3) 
        {
            break;
        }
        
        if (funcByteArray[i] == 0xB8)
        {            
            return *(funcByteArray + i + 1);
        }
    }

    return -1;
}

ULONG_PTR SSDTManager::GetFuncAddres(DWORD index)
{
    return reinterpret_cast<ULONG_PTR>(this->KiServiceTable + (*(this->KiServiceTable + index) >> 4));
}

BOOL SSDTManager::UnHook(PCHAR funcName)
{
    return 0;
}

void SSDTManager::FreeData()
{
    this->ntdll.Free();    
}

