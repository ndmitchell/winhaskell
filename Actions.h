
class Completion;

enum Actions
{
	actUnknown,
	actBlank,
	actExpression,
	actLoad,
	actShell,
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
};

void CommandsCompletion(Completion* c);
