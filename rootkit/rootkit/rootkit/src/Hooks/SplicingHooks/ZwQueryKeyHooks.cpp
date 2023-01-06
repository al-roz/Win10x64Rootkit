#include "ZwQueryKeyHooks.h"

NTSTATUS ZwQueryKeyHook_AnatherKeys(IN HANDLE KeyHandle, IN KEY_INFORMATION_CLASS KeyInformationClass, OUT PVOID KeyInformation, IN ULONG Length, OUT PULONG ResultLength)
{
	auto hookData = SplicingManager::getInstance().GetHookByAddres((ULONG_PTR)&ZwQueryKeyHook_AnatherKeys);

	auto originalFunc = reinterpret_cast<ZW_QUERY_KEY>(hookData.wrapper);

	auto status = originalFunc(KeyHandle, KeyInformationClass, KeyInformation, Length, ResultLength);

	if (!NT_SUCCESS(status))
	{
		return status;
	}	
	
	switch (KeyInformationClass)
	{
	case KEY_INFORMATION_CLASS::KeyFullInformation:
		{
			auto information = reinterpret_cast<PKEY_FULL_INFORMATION>(KeyInformation);

			RegistryKey key;

			if (!NT_SUCCESS(CheckReqForMatchRegisterInList(KeyHandle, key)))
			{
				return status;
			}

			ChangeInformation(information, key);
			
			break;
		}
	case KEY_INFORMATION_CLASS::KeyCachedInformation:
		{	
			auto information = reinterpret_cast<PKEY_CACHED_INFORMATION>(KeyInformation);							

			RegistryKey key;

			if (!NT_SUCCESS(CheckReqForMatchRegisterInList(KeyHandle, key)))
			{
				return status;
			}

			ChangeInformation(information, key);
			
			break;
		}	
	default:
		break;
	}	

	return status;
}
