#include "Header.h"

// Shared variables
HINSTANCE G_hInstance;
HWND      G_hWnd;


INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nCmdShow)
{
    InitCommonControls();
    LoadLibrary("RICHED32.DLL");

    // Save application instance
    G_hInstance = hInstance;
    ShowMainDialog();

	MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    return 0;
}
