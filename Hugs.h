
class Hugs: Interpreter
{
public:
    Hugs(LPCTSTR Command){Begin(Command);};
    ~Hugs(){};

    virtual void SetPrompt(LPCTSTR Command);
};

Hugs* StartHugs();

