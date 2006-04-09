
class GHCi: Interpreter
{
public:
    GHCi(LPCTSTR Command){Begin(Command);};
    ~GHCi(){};

    virtual void SetPrompt(LPCTSTR Command);
};

GHCi* StartGHCi();

