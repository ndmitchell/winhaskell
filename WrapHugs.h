
class WrapHugs : public Wrapper
{
private:
    bool Initialising;

    HANDLE hStdin, hStdout, hStderr;
    HANDLE hProcess;
    DWORD hProcessId;

    void InitDone();

public:
    WrapHugs();
    ~WrapHugs();

    void ReadStdErr();
    void ReadStdOut();

    virtual void RunCommand(LPCTSTR Command);
    virtual void AbortExecution();
};
