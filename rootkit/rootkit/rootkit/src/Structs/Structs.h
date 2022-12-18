#pragma once
#include <ntddk.h>
#include <ntimage.h>
#include <basetsd.h>
#include <windef.h>

#define PNULL reinterpret_cast<PVOID>(NULL)

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

    SSDTHook& operator= (DWORD dword)
    {
        HookFunc = 0;
        OriginalFunc = 0;
        return *this;
    }
};
