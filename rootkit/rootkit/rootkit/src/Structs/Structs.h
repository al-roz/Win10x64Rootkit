#pragma once
#include <ntddk.h>
#include <ntimage.h>
#include <basetsd.h>
#include <windef.h>

struct PeData
{
    ULONG_PTR startAddres;
    ULONGLONG ImageSize;

    PIMAGE_DOS_HEADER dosHeader;
    PIMAGE_NT_HEADERS ntHeader;

    PIMAGE_SECTION_HEADER sectionHeader;
    DWORD sectionsCount;

    PIMAGE_EXPORT_DIRECTORY exportDirectory;
};

struct ExportData
{
    DWORD ordinal;
    ULONG_PTR functionAddres;
    PCHAR name;
};

