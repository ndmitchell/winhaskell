

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

//private:
    HWND hWnd;

    HWND hStatus;
};
