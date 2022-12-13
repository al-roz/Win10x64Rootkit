#include "SSDTManager.h"


BOOL SSDTManager::InitializeInstanceData()
{
    if (!this->ntdll.InitializeFields(L"\\SystemRoot\\system32\\ntdll.dll"))
    {
        return FALSE;        
    }

    unsigned char KiSystemServiceStartPattern[] = {
    0x8B, 0xF8,
    0xC1, 0xEF, 0x07,
    0x83, 0xE7, 0x20,
    0x25, 0xFF, 0x0F, 0x00, 0x00
    };

    UINT32 signatureSize = 13;

    bool found = false;
    ULONG kiSSSOffset = 0;
    PeData kernelData = KernelManager::getInstance().GetKernelData();
    ULONG_PTR kernelBase = kernelData.startAddres;
    SIZE_T kernelSize = kernelData.ImageSize;

    

    for (; kiSSSOffset < kernelSize - signatureSize; kiSSSOffset++)
    {
        if (RtlCompareMemory(
            ((PBYTE)kernelBase + kiSSSOffset),
            KiSystemServiceStartPattern, signatureSize) == signatureSize)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        return FALSE;
    }

    ULONG_PTR addressAfterPattern = (ULONG_PTR)kernelBase + kiSSSOffset + signatureSize;
    ULONGLONG relativeOffset = 0;

    if ((*(unsigned char*)addressAfterPattern == 0x4c) &&
        (*(unsigned char*)(addressAfterPattern + 1) == 0x8d) &&
        (*(unsigned char*)(addressAfterPattern + 2) == 0x15))
    {
        relativeOffset = *(LONG*)(addressAfterPattern + 3);
    }

    if (relativeOffset == 0)
    {
        return FALSE;
    }

    this->ssdt = (SSDTStruct*)(addressAfterPattern + relativeOffset + 7);

    relativeOffset = 0;

    ULONG_PTR address = addressAfterPattern + 7;

    if ((*(unsigned char*)address == 0x4c) &&
        (*(unsigned char*)(address + 1) == 0x8d) &&
        (*(unsigned char*)(address + 2) == 0x1d))
    {
        relativeOffset = *(LONG*)(address + 3);
    }

    if (relativeOffset == 0)
    {
        return FALSE;
    }

    this->ssdtShadow = (SSDTStruct*)(address + relativeOffset + 7);    

    return TRUE;
}

