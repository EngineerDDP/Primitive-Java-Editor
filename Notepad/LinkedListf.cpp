#include "stdafx.h"
#include "Linkedlist.h"

LinkedList::LinkedList(const LinkedList & x)
{}
LinkedList::LinkedList()
{
	first = nullptr;
	last = nullptr;
	present = nullptr;
}
LinkedList::LinkedList(wchar_t x)
{
	first = new Chars(x, nullptr, nullptr);
	last = first;
	present = first;
}
LinkedList::~LinkedList()
{
	Search(TO_THE_LAST);
	while (Delete());
}
bool LinkedList::Add(wchar_t x)
{
	if (first == NULL)
	{
		present = new Chars(x, nullptr, nullptr);
		last = first = present;
	}
	else
	{
		Chars * temp;
		if (present != NULL)
		{
			if (present->next)
			{
				temp = present->next;
				present->next = new Chars(x, present, temp);
				temp->Set_Pos(present->next, SET_PREVIOUS);
			}
			else
			{
				present->next = new Chars(x, present, nullptr);
				last = present->next;
			}
			present = present->next;
		}
		else
		{
			present = new Chars(x, nullptr, first);
			first = present;
		}
	}
	return true;
}
bool LinkedList::Delete()
{
	if (present == NULL)
	{
		return false;
	}
	else
	{
		Chars * temp;
		if (present->previous && present->next)
		{
			temp = present;
			present->previous->Set_Pos(present->next, SET_NEXT);
			present->next->Set_Pos(present->previous, SET_PREVIOUS);
			present = present->previous;
		}
		else if (present->previous)
		{
			temp = present;
			present->previous->Set_Pos(nullptr, SET_NEXT);
			last = present = present->previous;
		}
		else if (present->next)
		{
			temp = present;
			present->next->Set_Pos(nullptr, SET_PREVIOUS);
			first = present->next;
			present = nullptr;
		}
		else
		{
			temp = present;
			present = first = last = nullptr;
		}
		delete temp;
		return true;
	}
}
bool LinkedList::Search(wchar_t x)
{
	present = first;
	for (; present->item == x;)
	{
		present = present->next;
		if (present == NULL)
			return false;
	}
	return true;
}
bool LinkedList::Search(int x)
{
	int i;
	if (first == NULL)
		return false;
	else if (x == TO_THE_LAST)
		present = last;
	else if (x == TO_THE_TOP)
		present = nullptr;
	else
		present = first;
	for (i = 1; i < x; i++)
	{
		present = present->next;
		if (present == nullptr)
		{
			present = last;
			return false;
		}
	}
	return true;
}
bool LinkedList::Read(wchar_t ** x)
{
	if (present != NULL)
	{
		*x = &(present->item);
		present = present->next;
		return true;
	}
	else
	{
		return false;
	}
}
wchar_t LinkedList::Read()
{
	if (present != NULL)
	{
		present = present->next;
		return present->previous->item;
	}
	else
	{
		return false;
	}
}