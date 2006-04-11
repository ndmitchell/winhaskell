
class WrapInteract : Wrapper
{
private:
    bool Initialising;

    HANDLE hStdin, hStdout, hStderr;
    HANDLE hProcess;
    DWORD hProcessId;

    void InitDone();

public:
    WrapInteract();
    virtual ~WrapInteract();

    void ReadStdErr();
    void ReadStdOut();

    virtual void RunCommand(LPCTSTR Command);
    virtual void AbortExecution();


    virtual void SetPrompt(LPCTSTR Command) = 0;
    virtual void 

};
}