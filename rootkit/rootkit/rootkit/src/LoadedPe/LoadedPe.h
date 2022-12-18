#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>

#include "../Structs/Structs.h"

class LoadedPe
{  
private:
    unsigned char* loadedBuffer;

    PeData data;

    NTSTATUS error;

    BOOLEAN InitializePeData();

    BOOLEAN InitializeLoadedBuffer(PWCHAR fileNameWC);

    ULONGLONG RvaToOffset(ULONGLONG rva);

public :

    BOOLEAN InitializeFields(PWCHAR fileNameWC);

    unsigned char* GetLoadedBuffer();

    PeData GetPeData();

    ImportData FindFunctionFromImportByName(PCHAR funcName);

    ExportData FindFunctionFromExportByName(PCHAR funcName);

    NTSTATUS GetError();

    void Free();   
    
};