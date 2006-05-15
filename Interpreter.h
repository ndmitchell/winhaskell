
class Action;
class String;
class Mutex;

enum OutputDest
{
    outNull,
    outBuffer,
    outPrompt,
    outScreen,
};

class Interpreter: private Console
{
public:
    Interpreter();
    ~Interpreter(){Abort();};

    void Begin(LPCTSTR Command);
    void AbortComputation();
    virtual LPCTSTR GetLastPrompt();

    // Some useful wrappers
    String* GetType(LPCTSTR Expression);

    // events provided by the child
	virtual void SetPrompt(LPCTSTR Prompt){};

    // is this message an error message
	virtual bool IsError(LPCTSTR Result){return false;};
    virtual bool ShowError(LPCTSTR Input, LPCTSTR Result){return false;};

	// Run an expression
	virtual bool Expression(LPCTSTR Expression);

	//return false is execution could not be started
	virtual bool Execute(Action* a);

protected:
    void Evaluate(LPCTSTR Line);
private:
    String* LastPrompt;

    //Send things directly to the output stream
    String* OutputBuffer;
    OutputDest dest;

    virtual void Read(LPCTSTR Buffer, DWORD Size, bool Stdout);
	virtual void Escape(ConsoleEscape Code, bool Stdout);
    virtual void Finished();

    bool Initialised;
    bool Executing;

    Mutex* mutex;
};
