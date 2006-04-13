
class Completion;

enum Actions
{
	actUnknown,
	actBlank,
	actExpression,
	actLoad,
	actShell,
	actQuit,
	actKnown,
};

class Action
{
public:
	Action(LPCTSTR Buffer);
	~Action();

	LPTSTR Orig;
	LPTSTR Command;
	LPTSTR Argument;

	Actions Code;
	LPCTSTR Help;

private:
	void PickBestAction();
};

void CommandsCompletion(Completion* c);
