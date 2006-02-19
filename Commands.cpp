#include "Header.h"
#include "Commands.h"
#include "Completion.h"

Command Commands[] =
{
    {true, "load", ":load <filenames>, load modules from specified files"},
    {true, "also", ":also <filenames>, read additional modules"},
    {true, "reload", ":reload, repeat last load command"},
    {true, "edit", ":edit, <filename>, edit file"},
    {true, "module", ":module <module>, set module for evaluating expressions"},
    {true, "type", ":type <expr>, print type of expression"},
    {true, "?", ":?, display this list of commands"},
    {true, "set", ":set <options>, set command line options"},
    {true, "names", ":names [pat], list names currently in scope"},
    {true, "info", ":info <names>, describe named objects"},
    {true, "browse", ":browse <modules>, browse names exported by <modules>"},
    {true, "main", ":main <aruments>, run the main function with the given arguments"},
    {true, "find", ":find <name>, edit module containing definition of name"},
    {true, "cd", ":cd dir, change directory"},
    {true, "version", ":version, print Hugs version"},
    {true, "quit", ":quit, exit Hugs interpreter"},
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

void CommandsCompletion(Completion* c)
{
    TCHAR Buffer[100];
    Buffer[0] = ':';

    for (int i = 0; Commands[i].Name != NULL; i++)
    {
        strcpy(&Buffer[1], Commands[i].Name);
        c->Add(Buffer);
    }
}
