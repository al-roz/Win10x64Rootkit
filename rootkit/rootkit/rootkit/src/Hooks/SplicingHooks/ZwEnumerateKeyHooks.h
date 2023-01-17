#pragma once
#include "../../SplicingManager/SplicingManager.h"
#include "../../SSDTManager/SSDTHooksManager.h"
#include "../../Structs/Structs.h"


typedef NTSTATUS(*ZW_ENUMERATE_KEY)(
    IN HANDLE  KeyHandle,
    IN ULONG  Index,
    IN KEY_INFORMATION_CLASS  KeyInformationClass,
    OUT PVOID  KeyInformation,
    IN ULONG  Length,
    OUT PULONG  ResultLength
    );

extern "C" NTSTATUS ZwEnumerateKeyHook_AnotherKeys(
    IN HANDLE  KeyHandle,
    IN ULONG  Index,
    IN KEY_INFORMATION_CLASS  KeyInformationClass,
    OUT PVOID  KeyInformation,
    IN ULONG  Length,
    OUT PULONG  ResultLength
);


