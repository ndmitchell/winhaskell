// Console
// Wraps round a windows Console
// Provides buffering
// Lowest level, users subclass this

const int ConsoleBufferSize = 1000;
const int ConsoleTimeout = 100;

const char ConsoleEscapeChar = 27;


enum ConsoleEscape
{
	conBackspace = -1, //send backspace as Escape, since thats easier
	conComplete = 50, //a custom escape character
	conReset = 0,
	conBold = 1,
	conUnderline = 4,
	conForeground = 30,
	conBackground = 40,
};


class Console
{
public:
    Console();
    ~Console();

    Console(LPCTSTR Command)
    {
        Start(Command);
    }

    //Write some data to the buffer, no terminator is written
    void Write(LPCTSTR Buffer, DWORD Size);

    //This method is called when some data is available to be read
    //This is a callback
    // true = stdout, false = stderr

	//RULES:
	//There will be a maximum of 0.1 seconds between data being received
	//and being written
	//escape characters must come in their entirity in 0.1 seconds, or are ignored
	//escape characters come at the front of a buffer only
    virtual void Read(LPCTSTR Buffer, DWORD Size, bool Stdout) = 0;
	virtual void Escape(ConsoleEscape Code, bool Stdout) = 0;

    //Called when the computation has terminated
    virtual void Finished() = 0;

    //Non-blocking
    void Start(LPCTSTR Command);

    //Blocking
    void Abort();

    //Send Ctrl+C, non-blocking
    void Exception();

	//Called by the app to denote that the timer has completed
	bool Tick();

    //Should be private, but because of marshalling is not
    void Internal_ReadHandle(bool Stdout);

private:
	//Lock
	HANDLE hSemaphore;
	void BeginLock();
	void EndLock();

	//Buffer
	bool BufStdout;
	TCHAR Buffer[ConsoleBufferSize+1];
	DWORD BufSize;
	void FlushBuffer();
	void ProcessEscape();

	//Flags
    bool Running;

	//Process handles
    HANDLE hStdin, hStdout, hStderr;
    HANDLE hProcess;
    DWORD hProcessId;
};
