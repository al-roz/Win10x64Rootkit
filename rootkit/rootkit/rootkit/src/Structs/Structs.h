#pragma once
#include "../stdafx.h"

#define PNULL reinterpret_cast<PVOID>(NULL)

typedef NTSTATUS(*IRP_HANDLER)(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
);

class WSTRING
{
public:
    ULONGLONG length;
    PWCHAR buffer;

    WSTRING() = default;

    WSTRING(PWCHAR buf, ULONGLONG len) : buffer(buf), length(len) {};
};

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
    DWORD IRP = MAXLONG;
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

#pragma pack(push,1) 
struct JAMPTAG
{
    unsigned short int jmp = 0x25ff;
    unsigned int tag = 0x00000000;
    ULONG_PTR addr;
};
#pragma pack(pop)


#pragma pack(push,1)
typedef struct _SYSTEM_THREADS {

    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
    ULONG ContextSwitchCount;
    //THREAD_STATE State;
    UCHAR State;
    KWAIT_REASON WaitReason;
    ULONG unknow1;

} SYSTEM_THREADS;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _SYSTEM_PROCESS {
    ULONG NextEntryDelta;
    ULONG ThreadCount;
    ULONG Reserved1[6];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ProcessName;
    KPRIORITY BasePriority;
    HANDLE ProcessId;
    ULONG InheritedFromProcessId;
    ULONG HandleCount;
    ULONG Reserved2[2];
    ULONG PrivatePageCount;
    VM_COUNTERS VmCounters;
    IO_COUNTERS IoCounters;
    SYSTEM_THREADS Threads[1];
} SYSTEM_PROCESS;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _IP_ADDR
{
    BYTE octets[4];
}IP_ADDR;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _INTERNAL_TCP_TABLE_SUBENTRY
{

    USHORT stub1;	//2
    USHORT port;
    ULONG ipAddr;
    char stub2[20];	// 

} INTERNAL_TCP_TABLE_SUBENTRY, * PINTERNAL_TCP_TABLE_SUBENTRY;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _INTERNAL_TCP_TABLE_ENTRY
{

    INTERNAL_TCP_TABLE_SUBENTRY localEntry;
    INTERNAL_TCP_TABLE_SUBENTRY remoteEntry;

} INTERNAL_TCP_TABLE_ENTRY, * PINTERNAL_TCP_TABLE_ENTRY;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _NSI_STATUS_ENTRY
{

    MIB_TCP_STATE state;
    ULONG stub1;
    ULONG stub2;
#if _WIN32_WINNT > 0x0600
    ULONG stub3;
#endif

} NSI_STATUS_ENTRY, * PNSI_STATUS_ENTRY;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _NSI_PARAM
{
    //
    // Total 70H size
    //
    ULONG_PTR UnknownParam1;
    ULONG_PTR UnknownParam2;
    void* UnknownParam3;
    ULONG_PTR UnknownParam4;
    ULONG UnknownParam5;
    ULONG UnknownParam6;
    PINTERNAL_TCP_TABLE_ENTRY tcpTable;
    ULONG_PTR typeInfo;
    ULONG_PTR UnknownParam9;
    ULONG_PTR UnknownParam10;
    PNSI_STATUS_ENTRY statusTable;
    ULONG_PTR UnknownParam12;
    void* UnknownParam13;
    ULONG_PTR UnknownParam14;
    ULONG_PTR tcpConnectionCount;

} NSI_PARAM, * PNSI_PARAM;
#pragma pack(pop)

struct Process
{
    DWORD PID;
    PCHAR Name;
};

struct SubKey
{
    PWCHAR subKeyName;
    ULONG subKeyNameLength;
};