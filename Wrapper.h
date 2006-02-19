
class Wrapper
{
public:
    bool Executing;

    Wrapper(){};
    virtual ~Wrapper(){};

    virtual void RunCommand(LPCTSTR Command) = 0;
    virtual void AbortExecution() = 0;
};

Wrapper* WrapperInit();
