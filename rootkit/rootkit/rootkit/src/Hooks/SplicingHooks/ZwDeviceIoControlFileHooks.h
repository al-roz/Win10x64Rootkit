#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>

#include "../../SplicingManager/SplicingManager.h"

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