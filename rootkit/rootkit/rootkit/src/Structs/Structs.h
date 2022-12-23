#pragma once
#include <ntddk.h>
#include <ntimage.h>
#include <basetsd.h>
#include <windef.h>

#define PNULL reinterpret_cast<PVOID>(NULL)

typedef NTSTATUS(*IRP_HANDLER)(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
);

struct PeData
{
    ULONG_PTR startAddres;
    ULONGLONG ImageSize;

    PIMAGE_DOS_HEADER dosHeader;
    PIMAGE_NT_HEADERS ntHeader;

    PIMAGE_SECTION_HEADER sectionHeader;
    DWORD sectionsCount;

    PIMAGE_EXPORT_DIRECTORY exportDirectory;

    PIMAGE_IMPORT_DESCRIPTOR importDescriptor;
};

struct ExportData
{
    DWORD ordinal;
    ULONG_PTR functionAddres;
    PCHAR name;
};

struct ImportData
{
    ULONG_PTR functionAddres;
    PCHAR name;
};

struct SSDTHook
{
    ULONG_PTR HookFunc = 0;
    ULONG_PTR OriginalFunc = 0;

    DWORD index = 0;
    PCHAR funcName = NULL;
};

struct SplicingHook
{
    ULONG_PTR HookFunc = 0;
    ULONG_PTR OriginalFunc = 0;
    ULONG_PTR wrapper = 0;
    PCHAR funcName = NULL;
};

struct IRPHook
{
    DWORD IRP = -1;
    IRP_HANDLER HookFunc = NULL;
    IRP_HANDLER OriginalFunc = NULL;
};

#pragma pack(push,1)
struct HOOKOPCODES
{
    unsigned short int mov = 0xB848;
    ULONG_PTR addr;
    unsigned char push = 0x50;
    unsigned char ret = 0xc3;
};
#pragma pack(pop)

#pragma pack(push,1)
struct SPLICINGOPCODES
{
    unsigned short int mov = 0xB848;
    ULONG_PTR addr;
    unsigned short int jmp = 0xe0ff;
};
#pragma pack(pop)
