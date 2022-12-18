#pragma once

#include <ntddk.h>
#include <windef.h>
#include <basetsd.h>
#include <stddef.h>

#include "../Structs/Structs.h"

template <typename Type>
void DoNothing(Type)
{

}


template <class Type>
class List
{
	LIST_ENTRY links;
	Type payload;

	void SetLinks(List<Type>* fElem, List<Type>* sElem)
	{
		fElem->links.Flink = &sElem->links;
		sElem->links.Blink = &fElem->links;
	}

	List<Type>* GenerateEmptyElement()
	{
		List<Type>* result = (List<Type>*)ExAllocatePool(PagedPool, sizeof(List<Type>));

		if (!result)
		{
			return NULL;
		}

		result->links.Blink = NULL;
		result->links.Flink = NULL;
		result->payload = NULL;
		return result;

	}

	List<Type>* GetLastElement()
	{
		LIST_ENTRY* entrys = &this->links;
		while (entrys->Flink != NULL)
		{
			entrys = entrys->Flink;
		}
		return CONTAINING_RECORD(entrys, List<Type>, links);
	}	

public:

	List()
	{
		this->links.Flink = NULL;
		this->links.Blink = NULL;
		this->payload = NULL;
	}

	void AddElement(Type newPayload)
	{
		auto lastElent = this->GetLastElement();
		auto newElemnt = this->GenerateEmptyElement();
		newElemnt->payload = newPayload;
		this->SetLinks(lastElent, newElemnt);
	}

	void DeleteElement(LIST_ENTRY* pLinks)
	{
		LIST_ENTRY* entrys = &this->links;
		while (entrys->Flink != NULL)
		{
			if (entrys == pLinks)
			{
								
			}

			entrys = entrys->Flink;
		}
		return CONTAINING_RECORD(entrys, List<Type>, links);
	}


	
	void FreeList(void(*freeFunc)(Type) = &DoNothing)
	{		
		LIST_ENTRY* next = this->links.Flink->Flink;
		List<Type>* tmpHead = CONTAINING_RECORD(this->links.Flink, List<Type>, links);
		
		while (next)
		{			
			freeFunc(tmpHead->payload);

			ExFreePool(tmpHead);
			tmpHead = CONTAINING_RECORD(next, List<Type>, links);
			next = tmpHead->links.Flink;
		}
	}

	
	void PrintList(void(*printFunc)(Type) = &DoNothing)
	{
		LIST_ENTRY* entrys = &this->links;

		while (entrys->Flink != NULL)
		{
			entrys = entrys->Flink;

			auto node = CONTAINING_RECORD(entrys, List<Type>, links);
			printFunc(node->payload);					
		}
	}
};