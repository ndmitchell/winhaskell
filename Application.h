
class Application;
class Toolbar;
class Interpreter;
class Compiler;
class Input;
class Output;
class RecentFiles;
enum Command;

extern Application* app;


// The main application for WinHaskell
class Application
{
public:
    Application();
    ~Application();

    void FireCommand(Command c, int Param);
    HMENU QueryCommand(Command c);
    void DefaultCommand();

    void ExecutionComplete();

    Toolbar* toolbar;
    Interpreter* interpreter;
    Compiler* compiler;
    Input* input;
    Output* output;
    RecentFiles* recentFiles;

//private:
    HWND hWnd;

    HWND hStatus;
};
