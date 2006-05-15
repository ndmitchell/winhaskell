
class NullEval: public Interpreter
{
public:
    NullEval(){};
    ~NullEval(){};

	virtual bool Expression(LPCTSTR Expression);
	virtual bool Execute(Action* a);
	virtual LPCTSTR GetLastPrompt(){return "<NULL eval> ";};
};

NullEval* StartNullEval();


