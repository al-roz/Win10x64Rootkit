#include "SSDTManager.h"


DWORD SSDTManager::FuncOffsetSSDT(ULONG_PTR func)
{
    return (func - reinterpret_cast<ULONG_PTR>(this->KiServiceTable)) << 4;
}

BOOL SSDTManager::InitializeInstanceData()
{
    if (!this->ntdll.InitializeFields(L"\\SystemRoot\\system32\\ntdll.dll"))
    {
        return FALSE;        
    }

    auto indexNtCreateFile = this->GetIndexSyscallFromNtdll("NtCreateFile");
    auto indexNtQuerySystemInformation = this->GetIndexSyscallFromNtdll("NtQuerySystemInformation");

    if (indexNtCreateFile == -1 || indexNtQuerySystemInformation == -1)
    {
        return FALSE;
    }

    ExportData realNtCreateFileFromExport = KernelManager::getInstance().FindFunctionFromExportByName("NtCreateFile");
    ExportData realNtQuerySystemInformation = KernelManager::getInstance().FindFunctionFromExportByName("NtQuerySystemInformation");      

    bool found = FALSE;
    ULONG kiServiceTableOffset = 0;
    PeData kernelData = KernelManager::getInstance().GetKernelData();
    ULONG_PTR kernelBase = kernelData.startAddres;
    SIZE_T kernelSize = kernelData.ImageSize;
    
    for (; kiServiceTableOffset < kernelSize; kiServiceTableOffset++)
    {
        PDWORD supposedKiServiceTable = reinterpret_cast<PDWORD>(kernelBase + kiServiceTableOffset);        

        auto cf = *(supposedKiServiceTable + indexNtCreateFile) >> 4;
        auto si = *(supposedKiServiceTable + indexNtQuerySystemInformation) >> 4;


        ULONG_PTR NtCreateFileAddres = reinterpret_cast<ULONG_PTR>((PBYTE)supposedKiServiceTable + cf);        
        ULONG_PTR NtQuerySystemInformationAddres = reinterpret_cast<ULONG_PTR>((PBYTE)supposedKiServiceTable + si);
        
        if ( realNtCreateFileFromExport.functionAddres == NtCreateFileAddres && 
             realNtQuerySystemInformation.functionAddres == NtQuerySystemInformationAddres)
        {
            found = TRUE;
            this->KiServiceTable = supposedKiServiceTable;            
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
            return *((PWORD)(funcByteArray + i + 1));
        }
    }

    return -1;
}

ULONG_PTR SSDTManager::GetFuncAddres(DWORD index)
{
    return reinterpret_cast<ULONG_PTR>((PBYTE)this->KiServiceTable + (this->KiServiceTable[index] >> 4));
}

BOOL SSDTManager::SetFuncInSSDT(DWORD index, ULONG_PTR funcAddres)
{
    auto irql = WPOFFx64();

    auto oldAddres = this->KiServiceTable[index];    
    this->KiServiceTable[index] = this->FuncOffsetSSDT(funcAddres) | (oldAddres & 0xF);       

    WPONx64(irql);

    return TRUE;
}



void SSDTManager::FreeData()
{
    this->ntdll.Free();    
}

