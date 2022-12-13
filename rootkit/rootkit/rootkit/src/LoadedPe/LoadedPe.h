#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>

#include "../Structs/Structs.h"

struct LoadedPe
{  
    unsigned char* loadedBuffer;

    PeData data;

    NTSTATUS error;

public :

    BOOLEAN InitializeFields(PWCHAR fileName);

    unsigned char* GetLoadedBuffer();

    PeData GetPeData();

    NTSTATUS GetError();

    void Free();

    
    
};