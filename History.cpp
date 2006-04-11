#include "Header.h"
#include "History.h"

// The history code
// Add to the history, get from the history with a delta
// pushing below the bottom one should return blank
// pushing above top should return top again

// the oldest history is at the lowest number

LPCSTR Blank = "";

History::History()
{
    Pos = 0;
    Count = 0;
}

void History::Add(LPCSTR Item)
{
    if (Count != 0 &&
		strcmp(Item, Items[Count-1]) == 0)
	{
		Pos = Count;
		return; //duplicate, eat it
    }

    if (Count == HistoryMax)
	{
		int i;
		free(Items[0]);
		for (i = 1; i < Count; i++)
			Items[i-1] = Items[i];
		Count--;
    }

    // there is now space for it
    Items[Count] = strdup(Item);
    Count++;
    Pos = Count;
}

LPCSTR History::Get(int Delta)
{
    // set a new value, with sanity checks
    Pos += Delta;
    if (Pos > Count)
		Pos = Count;
    if (Pos < 0)
		Pos = 0;

    if (Pos == Count)
		return Blank;
    else
		return Items[Pos];
}
