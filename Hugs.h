
class Hugs: Interpreter
{
public:
    Hugs(LPCTSTR Command){Begin(Command);};
    ~Hugs(){};

    virtual void SetPrompt(LPCTSTR Command);
    virtual bool IsError(LPCTSTR Result);
    virtual bool ShowError(LPCTSTR Input, LPCTSTR Result);
};

Hugs* StartHugs();

