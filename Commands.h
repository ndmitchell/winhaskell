
struct Command
{
    bool Real;
    LPCTSTR Name;
    LPCTSTR Help;
};

Command* GetCommand(LPCTSTR Cmd);
LPCTSTR GetArgument(LPCTSTR Cmd);
