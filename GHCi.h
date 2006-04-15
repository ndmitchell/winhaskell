
class GHCi: Interpreter
{
public:
    GHCi(LPCTSTR Command){Begin(Command);};
    ~GHCi(){};

    virtual void SetPrompt(LPCTSTR Command);
    virtual bool IsError(LPCTSTR Result){return false;}
};

GHCi* StartGHCi();

