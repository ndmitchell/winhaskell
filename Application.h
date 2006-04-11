
class Application;
class Toolbar;
class Interpreter;
class Compiler;
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

    Toolbar* toolbar;
    Interpreter* interpreter;
    Compiler* compiler;

//private:
    HWND hWnd;
};
