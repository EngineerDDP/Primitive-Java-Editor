#pragma once

#define TO_THE_LAST -1
#define TO_THE_TOP 0
#define TO_THE_FIRST 1
#define SET_PREVIOUS true
#define SET_NEXT false

struct Chars
{
public:
	wchar_t item;
	Chars * previous;
	Chars * next;
	Chars(wchar_t c, Chars * previous, Chars * next)
	{
		item = c;
		this->next = next;
		this->previous = previous;
	}
	void Set_Pos(Chars * previous, Chars * next)
	{
		this->previous = previous;
		this->next = next;
	}
	void Set_Pos(Chars * temp, bool x)
	{
		if (x)
			this->previous = temp;
		else
			this->next = temp;
	}
};

class LinkedList
{
private:
	Chars * first;
	Chars * last;
	Chars * present;
	LinkedList(const LinkedList &);
public:
	LinkedList();
	LinkedList(wchar_t x);
	~LinkedList();
	bool Add(wchar_t x);
	bool Delete();
	bool Search(wchar_t x);
	bool Search(int x);
	bool Read(wchar_t ** x);
	wchar_t Read();
};