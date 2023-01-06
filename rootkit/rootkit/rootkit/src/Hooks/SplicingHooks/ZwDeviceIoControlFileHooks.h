#pragma once
#include "../../SplicingManager/SplicingManager.h"
#include "../../SSDTManager/SSDTHooksManager.h"
#include "../../Structs/Structs.h"

#define HTONS(a)  (((0xFF&a)<<8) + ((0xFF00&a)>>8)) 
#define IP_ADDR(a,b,c,d) ((a << 24) + (b << 16) + (c << 8) + d)

typedef NTSTATUS(*ZW_DEVICE_IO_CONTROL_FILE)(
    IN HANDLE  FileHandle,
    IN HANDLE  Event,
    IN PIO_APC_ROUTINE  ApcRoutine,
    IN PVOID  ApcContext,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  IoControlCode,
    IN PVOID  InputBuffer,
    IN ULONG  InputBufferLength,
    OUT PVOID  OutputBuffer,
    IN ULONG  OutputBufferLength
    );

extern "C" NTSTATUS ZwDeviceIoControlFileHook_AddElement(
    IN HANDLE  FileHandle,
    IN HANDLE  Event,
    IN PIO_APC_ROUTINE  ApcRoutine,
    IN PVOID  ApcContext,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  IoControlCode,
    IN PVOID  InputBuffer,
    IN ULONG  InputBufferLength,
    OUT PVOID  OutputBuffer,
    IN ULONG  OutputBufferLength
);
