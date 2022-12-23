#pragma once

#include <ntddk.h>
#include <windef.h>
#include <basetsd.h>

#include "../../Structs/Structs.h"
#include "../../NtfsIRPManager/NtfsIRPManager.h"

NTSTATUS HookDirectoryControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
