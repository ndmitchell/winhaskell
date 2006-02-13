
#include "Header.h"

void RegReadStr(LPCTSTR Key, LPCTSTR Default, LPTSTR Buffer)
{
    HKEY hKey;
    bool Success = false;
    if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Haskell\\WinHaskell", &hKey) == ERROR_SUCCESS)
    {
	    LONG Size = MAX_PATH;
	    Success = (RegQueryValue(hKey, Key, Buffer, &Size) == ERROR_SUCCESS);
	    RegCloseKey(hKey);
    }
    if (!Success)
    	strcpy(Buffer, Default);
}

LPTSTR RegRegStrDup(LPCTSTR Key, LPCTSTR Default)
{
    TCHAR Buffer[MAX_PATH];
    RegReadStr(Key, Default, Buffer);
    return strdup(Buffer);
}

void RegWriteStr(LPCTSTR Key, LPCTSTR Value)
{
    HKEY hKey;
    if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Haskell\\WinHaskell", &hKey) == ERROR_SUCCESS)
    {
	    RegSetValue(hKey, Key, REG_SZ, Value, (DWORD) strlen(Value));
	    RegCloseKey(hKey);
    }
}

int RegReadInt(LPCTSTR Key, int Default)
{
    char Buf1[100], Buf2[100];
    itoa(Default, Buf1, 10); 
    RegReadStr(Key, Buf1, Buf2);
    return atoi(Buf2);
}

void RegWriteInt(LPCTSTR Key, int Value)
{
    char Buf[100];
    itoa(Value, Buf, 10);
    RegWriteStr(Key, Buf);
}

void RegistryReadMru(char** Buffer)
{
    int i;
    TCHAR Buf[5] = "Mru1";

    for (i = 0; ; i++)
    {
	    LPTSTR Res;
	    Buf[3] = i + '0';
        Res = RegRegStrDup(Buf, "");

	    Buffer[i] = Res;
	    if (Res[0] == '\0')
        {
	        Buffer[i] = NULL;
	        free(Res);
	        break;
	    }
    }
}

void RegistryWriteMru(char** Buffer)
{
    int i;
    char Buf[5] = "Mru1";

    for (i = 0; Buffer[i] != NULL; i++)
    {
	    Buf[3] = i + '0';
        RegWriteStr(Buf, Buffer[i]);
    }
}

void RegistryReadFont(CHARFORMAT* cf)
{
    cf->cbSize = sizeof(CHARFORMAT);
    cf->dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC | CFM_SIZE;
    cf->dwEffects = 0;

    RegReadStr("FontName", "Courier New", cf->szFaceName);
    cf->yHeight = RegReadInt("FontSize", PointToTwip(10));
    if (RegReadInt("FontWeight", 0)) cf->dwEffects |= CFE_BOLD;
    if (RegReadInt("FontItalic", 0)) cf->dwEffects |= CFE_ITALIC;
}

void RegistryWriteFont(CHARFORMAT* cf)
{
    RegWriteStr("FontName", cf->szFaceName);
    RegWriteInt("FontSize", cf->yHeight);
    RegWriteInt("FontWeight", (cf->dwEffects & CFE_BOLD ? 1 : 0));
    RegWriteInt("FontItalic", (cf->dwEffects & CFE_ITALIC ? 1 : 0));
}

void RegistryReadWindowPos(HWND hWnd)
{
    int x, y, cx, cy;
    int Maximized = RegReadInt("WindowMaximized", 1);

    if (Maximized) {
	    ShowWindow(hWnd, SW_MAXIMIZE);
	    return;
    }

    x = RegReadInt("WindowLeft", -1);
    y = RegReadInt("WindowTop", -1);
    cx = RegReadInt("WindowWidth", -1);
    cy = RegReadInt("WindowHeight", -1);

    if (x == -1) return;

    SetWindowPos(hWnd, NULL, x, y, cx, cy, SWP_NOZORDER);
}

void RegistryWriteWindowPos(HWND hWnd)
{
    RECT rc;
    int Maximized;

    // The user has closed while the app is minimized
    // The current values are wrong, who knows what the correct
    // ones are, so just be safe and store nothing
    if (IsIconic(hWnd))
	    return;

    Maximized = (IsZoomed(hWnd) ? 1 : 0);
    RegWriteInt("WindowMaximized", Maximized);

    if (Maximized)
	    return;

    GetWindowRect(hWnd, &rc);
    RegWriteInt("WindowLeft", rc.left);
    RegWriteInt("WindowTop", rc.top);
    RegWriteInt("WindowWidth", rc.right - rc.left);
    RegWriteInt("WindowHeight", rc.bottom - rc.top);
}
