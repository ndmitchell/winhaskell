#include "Header.h"
#include "String.h"

String::String()
{
    Buffer = NULL;
}

String::String(LPCTSTR Value)
{
    SetInternal(Value);
}

void String::SetInternal(LPCTSTR Value)
{
    Size = (int) strlen(Value);
    BufSize = Size;
    Buffer = new TCHAR[BufSize+1];
    strcpy(Buffer, Value);
}

String::~String()
{
    delete[] Buffer;
}

void String::Set(LPCTSTR Value)
{
    delete[] Buffer;
    SetInternal(Value);
}

void String::Append(LPCTSTR Value)
{
    if (Buffer == NULL)
    {
        SetInternal(Value);
    }
    else
    {
        int Len = (int) strlen(Value);
        if (Size + Len > BufSize)
        {
            //realloc
            BufSize = max(Size+Len, Size*2);
            LPTSTR NewBuf = new TCHAR[BufSize+1];
            memcpy(NewBuf, Buffer, Size);
            delete[] Buffer;
            Buffer = NewBuf;
        }
        strcpy(&Buffer[Size], Value);
    }
}
