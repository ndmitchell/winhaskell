#include "Header.h"
#include "Commands.h"

Command Commands[] =
{
    {true, "type", "Get a type"},
    {true, "load", "Load a module"},
    {true, "main", "Run the main command"},
    {true, NULL, NULL}
};

Command CmdUnknown = {false, "unknown", "Unknown command"};
Command CmdEmpty = {false, "blank", "Type a command now"};

bool IsEmpty(TCHAR c)
{
    return (c == 0 || isspace(c));
}

Command* GetCommand(LPCTSTR Cmd)
{
    LPCTSTR c = Cmd;
    while (isspace(*c))
        c++;
    if (*c != ':')
        return NULL;

    int BestMatch = 0;
    Command* BestCommand = (IsEmpty(c[1]) ? &CmdEmpty : &CmdUnknown);

    for (int i = 0; Commands[i].Name != NULL; i++)
    {
        int Match;
        for (Match = 0;
            Commands[i].Name[Match] != 0 &&
            tolower(Commands[i].Name[Match]) == tolower(c[Match+1]);
            Match++)
        {
            ; // Nothing
        }

        if (IsEmpty(c[Match+1]) && (Match > BestMatch))
        {
            BestMatch = Match;
            BestCommand = &Commands[i];
        }
    }
    return BestCommand;
}

LPCTSTR GetArgument(LPCTSTR Cmd)
{
    return NULL;
}

