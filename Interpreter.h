
class Action;

class Interpreter: private Console
{
public:
    Interpreter();
    ~Interpreter(){Abort();};

    void Begin(LPCTSTR Command);
    void AbortComputation();


    // events provided by the child
    virtual void SetPrompt(LPCTSTR Prompt) = 0;

	void Evaluate(LPCTSTR Expression);
	//return false is execution could not be started
	bool Execute(Action* a);

private:

    virtual void Read(LPCTSTR Buffer, DWORD Size, bool Stdout);
	virtual void Escape(ConsoleEscape Code, bool Stdout);
    virtual void Finished();

    bool Initialised;
    bool Executing;
};
