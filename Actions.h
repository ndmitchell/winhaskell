
class Completion;

struct Action
{
    bool Real;
    LPCTSTR Name;
    LPCTSTR Help;
};

Action* GetCommand(LPCTSTR Cmd);
LPCTSTR GetArgument(LPCTSTR Cmd);
void CommandsCompletion(Completion* c);
