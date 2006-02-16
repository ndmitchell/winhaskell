#include "Header.h"
#include "Commands.h"

LPCTSTR CmdNames[] =
{
    "type", "load", "main", NULL
};

LPCTSTR CmdHelp[] =
{
    "Get a type",
    "Load a module",
    "Run the main command",
    "Type a command now",
    "Unknown command",
};

bool IsEmpty(TCHAR c)
{
    return (c == 0 || isspace(c));
}

Commands GetCommand(LPCTSTR Cmd)
{
    LPCTSTR c = Cmd;
    while (isspace(*c))
        c++;
    if (*c != ':')
        return CmdNone;

    int BestMatch = 0;
    Commands BestCommand = CmdUnknown;
    for (int i = 0; CmdNames[i] != NULL; i++)
    {
        int Match;
        for (Match = 0;
            CmdNames[i][Match] != 0 &&
            tolower(CmdNames[i][Match]) == tolower(c[Match+1]);
            Match++)
        {
            ; // Nothing
        }

        if (IsEmpty(c[Match+1]) && (Match > BestMatch))
        {
            BestMatch = Match;
            BestCommand = (Commands) i;
        }
    }

    if (BestCommand == CmdUnknown && IsEmpty(c[1]))
        return CmdEmpty;

    return BestCommand;
}

LPCTSTR GetArgument(LPCTSTR Cmd)
{
    return NULL;
}

LPCTSTR GetCommandHelp(Commands Cmd)
{
    return CmdHelp[Cmd];
}

