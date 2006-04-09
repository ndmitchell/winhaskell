// Console
// Wraps round a windows Console
// Provides buffering
// Lowest level, users subclass this

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
    virtual void Read(LPTSTR Buffer, DWORD Size, bool Stdout) = 0;

    //Called when the computation has terminated
    virtual void Finished() = 0;

    //Non-blocking
    void Start(LPCTSTR Command);

    //Blocking
    void Abort();

    //Send Ctrl+C, non-blocking
    void Exception();


    //Should be private, but because of marshalling is not
    void Internal_ReadHandle(bool Stdout);

private:
    bool Running;

    HANDLE hStdin, hStdout, hStderr;
    HANDLE hProcess;
    DWORD hProcessId;
};
