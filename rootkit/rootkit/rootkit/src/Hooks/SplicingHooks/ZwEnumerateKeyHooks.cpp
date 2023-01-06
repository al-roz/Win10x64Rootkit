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

NTSTATUS ZwEnumerateKeyHook_AnatherKeys(IN HANDLE KeyHandle, IN ULONG Index, IN KEY_INFORMATION_CLASS KeyInformationClass, OUT PVOID KeyInformation, IN ULONG Length, OUT PULONG ResultLength)
{
	auto hookData = SplicingManager::getInstance().GetHookByAddres((ULONG_PTR)&ZwEnumerateKeyHook_AnatherKeys);

	auto originalFunc = reinterpret_cast<ZW_ENUMERATE_KEY>(hookData.wrapper);

	auto status = originalFunc(KeyHandle, Index, KeyInformationClass, KeyInformation, Length, ResultLength);

	RegistryKey key;

	if (!NT_SUCCESS(CheckReqForMatchRegisterInList(KeyHandle, key)))
	{
		return status;
	}

	auto node = key.subkeysList.GetNodeByIndex(Index);

	if (!NT_SUCCESS(FixMemmory(KeyInformationClass, Length, node.subKeyNameLength, ResultLength)))
	{
		return STATUS_BUFFER_TOO_SMALL;
	}

	switch(KeyInformationClass)
	{
	case KEY_INFORMATION_CLASS::KeyBasicInformation:
	{
		break;
	}
	case KEY_INFORMATION_CLASS::KeyNodeInformation:
	{
		break;
	}
	case KEY_INFORMATION_CLASS::KeyFullInformation:
	{
		break;
	}
	}

	return status;
}
