
class Console;

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

	//Timer, for console only
	//Bug: Should support many consoles, currently supports only one
	void AddTimer(Console* c, UINT Elapse);
	void DelTimer(Console* c);

//private:
	Console* timeout;
    HWND hWnd;

    HWND hStatus;
};
