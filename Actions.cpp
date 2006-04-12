#include "Header.h"
#include "Actions.h"
#include "Completion.h"

struct ActionItem
{
	Actions Code;
    LPCTSTR Name;
    LPCTSTR Help;
};

ActionItem Commands[] =
{
    {actLoad, "load", ":load <filenames>, load modules from specified files"},
    {actKnown, "also", ":also <filenames>, read additional modules"},
    {actKnown, "reload", ":reload, repeat last load command"},
    {actKnown, "edit", ":edit, <filename>, edit file"},
    {actKnown, "module", ":module <module>, set module for evaluating expressions"},
    {actKnown, "type", ":type <expr>, print type of expression"},
    {actKnown, "?", ":?, display this list of commands"},
    {actKnown, "set", ":set <options>, set command line options"},
    {actKnown, "names", ":names [pat], list names currently in scope"},
    {actKnown, "info", ":info <names>, describe named objects"},
    {actKnown, "browse", ":browse <modules>, browse names exported by <modules>"},
    {actKnown, "main", ":main <aruments>, run the main function with the given arguments"},
    {actKnown, "find", ":find <name>, edit module containing definition of name"},
    {actKnown, "cd", ":cd dir, change directory"},
    {actKnown, "version", ":version, print Hugs version"},
    {actKnown, "quit", ":quit, exit Hugs interpreter"},
    {actUnknown, NULL, NULL}
};

bool IsEmpty(TCHAR c)
{
    return (c == 0 || isspace(c));
}

Action::~Action()
{
	free(Orig);
	if (Command != NULL) free(Command);
	if (Argument != NULL) free(Argument);
}

Action::Action(LPCTSTR Cmd)
{
	Orig = strdup(Cmd);

    LPCTSTR c = Cmd;
    while (isspace(*c))
        c++;

	if (c[0] == '!')
	{
		Command = NULL;
		Argument = strdup(&c[1]);
		Code = actShell;
		Help = "Run the following shell command";
		return;
	}
	else if (c[0] == 0)
	{
		Command = NULL;
		Argument = NULL;
		Code = actBlank;
		Help = "Type an expression or a :command";
		return;
	}
	else if (c[0] != ':')
	{
		Command = NULL;
		Argument = strdup(c);
		Code = actExpression;
		Help = "Evaluate a Haskell expression";
		return;
	}

    int BestMatch = 0;
    ActionItem* BestCommand = NULL;

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

	if (BestCommand == NULL)
	{
		Command = strdup(c);
		Argument = NULL;
		Code = actUnknown;
		Help = "This command is unknown by WinHaskell";
	}
	else
	{
		Command = strdup(BestCommand->Name);
		Argument = NULL;
		Code = BestCommand->Code;
		Help = BestCommand->Help;
	}
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
