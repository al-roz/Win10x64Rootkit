#pragma once
#include <ntddk.h>
#include <aux_klib.h>
#include <basetsd.h>
#include <windef.h>



BOOLEAN GetNtoskrnl(ULONG_PTR* kernelBase, SIZE_T* kernelSize);

