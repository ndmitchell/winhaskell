
class Interpreter: private Console
{
public:
    Interpreter();
    ~Interpreter(){Abort();};

    void Begin(LPCTSTR Command);
    void Evaluate(LPCTSTR Line);
    void AbortComputation();


    // events provided by the child
    virtual void SetPrompt(LPCTSTR Prompt) = 0;

private:

    virtual void Read(LPTSTR Buffer, DWORD Size, bool Stdout);
    virtual void Finished();

    bool Initialised;
    bool Executing;
};