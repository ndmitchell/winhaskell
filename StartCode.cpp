#include "Header.h"
#include "Application.h"

// Shared variables
HINSTANCE hInst;

Application* app;
Toolbar* toolbar;
Interpreter* interpreter;
Compiler* compiler;
Input* input;
Output* output;
RecentFiles* recentFiles;
History* history;


INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nCmdShow)
{
    InitCommonControls();
    LoadLibrary("RICHED32.DLL");

    // Save application instance
    hInst = hInstance;
    app = new Application();

	MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    return 0;
}
