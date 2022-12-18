#include "KernelManager.h"

BOOLEAN KernelManager::InitializeInstanceData()
{    
    GetNtoskrnl(&kernelData.startAddres, &kernelData.ImageSize);   

    kernelData.dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(kernelData.startAddres);

    if (kernelData.dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Error DOS signature");
        return FALSE;
    }

    kernelData.ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(kernelData.startAddres + kernelData.dosHeader->e_lfanew);

    if (kernelData.ntHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Error NT signature");
        return FALSE;
    }

    kernelData.sectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(
        reinterpret_cast<PBYTE>(&kernelData.ntHeader->OptionalHeader) + kernelData.ntHeader->FileHeader.SizeOfOptionalHeader);

    kernelData.sectionsCount = kernelData.ntHeader->FileHeader.NumberOfSections;

    kernelData.exportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
        kernelData.startAddres + kernelData.ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    return TRUE;
}

PeData KernelManager::GetKernelData()
{
    return this->kernelData;
}

void KernelManager::PrintExport()
{
    auto exp = this->kernelData.exportDirectory;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "**********  Export Table:\n");    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "num fun: %d\n", exp->NumberOfFunctions);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "num names: %d\n", exp->NumberOfNames);

    auto functionsArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfFunctions);

    auto namesArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfNames);

    auto nameOrdinalsArray = (WORD*)(this->kernelData.startAddres + exp->AddressOfNameOrdinals);

    for (ULONG i = 0; i < exp->NumberOfNames; ++i)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d ", nameOrdinalsArray[i] + exp->Base);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%-40s\t", (PCHAR)(this->kernelData.startAddres + namesArray[i]));
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%p\n", (WORD*)(this->kernelData.startAddres + functionsArray[nameOrdinalsArray[i]]));
    }
}

void KernelManager::PrintSections()
{
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "**********  Sections Table\n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "name      VirtAddr  VirtSize  RawAddr   RawSize   Character\n");

    auto secCount = this->kernelData.sectionsCount;
    auto sections = this->kernelData.sectionHeader;

    for (ULONG i = 0; i < secCount; i++)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%-8s\t", &sections[i].Name);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%lu\t", sections[i].VirtualAddress);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%lu\t", sections[i].Misc.VirtualSize);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%lu\t", sections[i].PointerToRawData);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%lu\t", sections[i].SizeOfRawData);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%lu\n", sections[i].Characteristics);
    }

}

ExportData KernelManager::FindFunctionFromExportByName(PCHAR funcName)
{
    auto exp = this->kernelData.exportDirectory;

    auto functionsArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfFunctions);

    auto namesArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfNames);

    auto nameOrdinalsArray = (WORD*)(this->kernelData.startAddres + exp->AddressOfNameOrdinals);

    for (ULONG i = 0; i < exp->NumberOfNames; ++i)
    {  
        if (_stricmp(funcName, (PCHAR)(this->kernelData.startAddres + namesArray[i])) == 0)
        {
            return ExportData{ nameOrdinalsArray[i] + exp->Base,
                               (this->kernelData.startAddres + functionsArray[nameOrdinalsArray[i]]),
                               reinterpret_cast<PCHAR>(this->kernelData.startAddres + namesArray[i]) };
        }
    }

    return ExportData{ NULL, NULL, NULL };
}

ExportData KernelManager::FindFunctionFromExportByAddres(ULONG_PTR funcAddr)
{
    auto exp = this->kernelData.exportDirectory;

    auto functionsArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfFunctions);

    auto namesArray = (DWORD*)(this->kernelData.startAddres + exp->AddressOfNames);

    auto nameOrdinalsArray = (WORD*)(this->kernelData.startAddres + exp->AddressOfNameOrdinals);

    for (ULONG i = 0; i < exp->NumberOfNames; ++i)
    {
        if (funcAddr ==  (this->kernelData.startAddres + functionsArray[nameOrdinalsArray[i]]))
        {
            return ExportData{ nameOrdinalsArray[i] + exp->Base,
                               (this->kernelData.startAddres + functionsArray[nameOrdinalsArray[i]]),
                               reinterpret_cast<PCHAR>(this->kernelData.startAddres + namesArray[i]) };
        }
    }

    return ExportData{ NULL, NULL, NULL };
}
