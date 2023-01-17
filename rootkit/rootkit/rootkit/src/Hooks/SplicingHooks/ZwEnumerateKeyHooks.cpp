#include "ZwEnumerateKeyHooks.h"

NTSTATUS FixMemmory(KEY_INFORMATION_CLASS KeyInformationClass,ULONG systemLength, ULONG nameLenght, PULONG ResultLength)
{
	switch (KeyInformationClass)
	{
	case KEY_INFORMATION_CLASS::KeyBasicInformation:
		{

			if (systemLength < sizeof(KEY_BASIC_INFORMATION) + nameLenght)
			{
				if (ResultLength != NULL)
				{
					*ResultLength = sizeof(KEY_BASIC_INFORMATION) + nameLenght;
				}
				return STATUS_BUFFER_TOO_SMALL;
			}			
		}
	case KEY_INFORMATION_CLASS::KeyNodeInformation:
		{
			if (systemLength < sizeof(KEY_NODE_INFORMATION) + nameLenght)
			{
				if (ResultLength != NULL)
				{
					*ResultLength = sizeof(KEY_NODE_INFORMATION) + nameLenght;
				}
				return STATUS_BUFFER_TOO_SMALL;
			}			
		}
	case KEY_INFORMATION_CLASS::KeyFullInformation:
		{
			if (systemLength < sizeof(KEY_FULL_INFORMATION) + nameLenght)
			{
				if (ResultLength != NULL)
				{
					*ResultLength = sizeof(KEY_FULL_INFORMATION) + nameLenght;
				}
				return STATUS_BUFFER_TOO_SMALL;
			}			
		}	
	}
	return STATUS_SUCCESS;
}

NTSTATUS ZwEnumerateKeyHook_AnotherKeys(IN HANDLE KeyHandle, IN ULONG Index, IN KEY_INFORMATION_CLASS KeyInformationClass, OUT PVOID KeyInformation, IN ULONG Length, OUT PULONG ResultLength)
{
	auto hookData = SplicingManager::getInstance().GetHookByAddres(reinterpret_cast<ULONG_PTR>(&ZwEnumerateKeyHook_AnotherKeys));

	auto originalFunc = reinterpret_cast<ZW_ENUMERATE_KEY>(hookData.wrapper);

	RegistryKey key;

	if (!NT_SUCCESS(CheckReqForMatchRegisterInList(KeyHandle, key)))
	{
		auto status = originalFunc(KeyHandle, Index, KeyInformationClass, KeyInformation, Length, ResultLength);
		return status;
	}

	if ((SIZE_T)Index + 1 > key.subkeysList.Size())
	{
		return STATUS_NO_MORE_ENTRIES;
	}
	DbgBreakPoint();
	auto node = key.subkeysList.GetNodeByIndex(Index);

	if (!NT_SUCCESS(FixMemmory(KeyInformationClass, Length, node.subKeyNameLength, ResultLength)))
	{
		return STATUS_BUFFER_TOO_SMALL;
	}

	switch(KeyInformationClass)
	{
	case KEY_INFORMATION_CLASS::KeyBasicInformation:
	{
		auto info = reinterpret_cast<PKEY_BASIC_INFORMATION>(KeyInformation);

		info->LastWriteTime.QuadPart = 0;
		RtlCopyMemory(&info->Name, node.subKeyName, node.subKeyNameLength);
		info->NameLength = node.subKeyNameLength;
		info->TitleIndex = 0;

		break;
	}
	case KEY_INFORMATION_CLASS::KeyNodeInformation:
	{
		auto info = reinterpret_cast<PKEY_NODE_INFORMATION>(KeyInformation);

		info->ClassLength = 0;
		info->ClassOffset = -1;
		info->LastWriteTime.QuadPart = 0;
		RtlCopyMemory(&info->Name, node.subKeyName, node.subKeyNameLength);
		info->NameLength = node.subKeyNameLength;
		info->TitleIndex = 0;

		break;
	}
	case KEY_INFORMATION_CLASS::KeyFullInformation:
	{
		auto info = reinterpret_cast<PKEY_FULL_INFORMATION>(KeyInformation);

		info->Class[0] = 0;
		info->ClassLength = 0;
		info->ClassOffset = -1;
		info->LastWriteTime.QuadPart = 0;
		info->MaxClassLen = 255;
		info->MaxNameLen = 255;
		info->MaxValueDataLen = 255;
		info->MaxValueNameLen = 0;
		info->SubKeys = 0;

		break;
	}
	default:
		break;
	}
	return STATUS_SUCCESS;
}
