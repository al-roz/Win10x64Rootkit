#pragma once
#include "../stdafx.h"
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

	SIZE_T size;

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
		links.Flink = NULL;
		links.Blink = NULL;
		size = 0;
	}

	void AddElement(Type newPayload)
	{
		auto lastElent = this->GetLastElement();
		auto newElemnt = this->GenerateEmptyElement();
		newElemnt->payload = newPayload;
		this->SetLinks(lastElent, newElemnt);
		++size;
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
		--size;
		return CONTAINING_RECORD(entrys, List<Type>, links);
	}


	
	void FreeList(void(*freeFunc)(Type) = &DoNothing)
	{		
		
		List<Type>* tmpHead = CONTAINING_RECORD(this->links.Flink, List<Type>, links);

		
		while (tmpHead)
		{	
			auto next = CONTAINING_RECORD(tmpHead->links.Flink, List<Type>, links);
			freeFunc(tmpHead->payload);
			ExFreePool(tmpHead);

			tmpHead = next;
		}
		
	}

	template <typename Func>
	Type Find(Func findFunc)
	{
		LIST_ENTRY* entrys = &this->links;

		while (entrys->Flink != NULL)
		{
			entrys = entrys->Flink;

			auto node = CONTAINING_RECORD(entrys, List<Type>, links);

			if (findFunc(node->payload))
			{
				return node->payload;
			}
		}

		return Type();
	}

	

	template <typename Func>
	BOOL Ñontains(Func findFunc)
	{
		LIST_ENTRY* entrys = &this->links;

		while (entrys->Flink != NULL)
		{
			entrys = entrys->Flink;

			auto node = CONTAINING_RECORD(entrys, List<Type>, links);

			if (findFunc(node->payload))
			{
				return TRUE;
			}
		}

		return FALSE;
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

	SIZE_T Size()
	{
		return this->size;
	}

	Type GetNodeByIndex(SIZE_T index)
	{
		List<Type>* tmpHead = CONTAINING_RECORD(this->links.Flink, List<Type>, links);

		int i = 0;
		while (i != index)
		{			
			tmpHead = CONTAINING_RECORD(tmpHead->links.Flink, List<Type>, links);;
			i++;
		}

		return tmpHead;
	}
};