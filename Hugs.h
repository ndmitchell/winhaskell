
class Hugs: Interpreter
{
public:
    Hugs(LPCTSTR Command){Begin(Command);};
    ~Hugs(){};

    virtual void SetPrompt(LPCTSTR Command);
    virtual bool IsError(LPCTSTR Result);
    virtual void ErrorHints(LPCTSTR Text, LPCTSTR Result);
};

Hugs* StartHugs();

