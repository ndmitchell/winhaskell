
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

	void LoadFile(LPCTSTR File);

	void CommandRun(LPCTSTR File);
	void CommandRun();
	void CommandOpen();

    void RunningChanged(bool Running);

	//Timer, for console only
	//Bug: Should support many consoles, currently supports only one
	void AddTimer(Console* c, UINT Elapse);
	void DelTimer(Console* c);

	void StatusBar(LPCTSTR Text);
	void Warning(LPCTSTR Text);

    void AddError(int Start, int Length, LPCTSTR Message);

//private:
	Console* timeout;
	UINT TimerId;
    HWND hWnd;

    HWND hStatus;
};
