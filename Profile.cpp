#include "Header.h"
#include "Application.h"
#include "Output.h"
#include "RecentFiles.h"
#include "Console.h"

class Profile : public Console
{
public:
	int State;
	LPCTSTR Arguments;
	virtual void Finished();
};

void Profile::Finished()
{
	if (State == 0)
	{
		output->SetForecolor(BLUE);
		output->Append("Finished compiling\n");
		output->SetForecolor(BLUE);
		output->Append("Executing for profiling\n");

		State = 1;
		TCHAR Buffer[500];
		wsprintf(Buffer, "Main +RTS -p -RTS %s", Arguments);
		output->SetForecolor(BLACK);
		Start(Buffer);
	}
	else if (State == 1)
	{
		output->Append("Finished executing\n");
		app->RunningChanged(false);
	}
}

bool ProfileRun(LPCTSTR Arguments)
{
	if (recentFiles->Last() == NULL)
	{
		app->Warning("Error, must load a file before running profile");
		return false;
	}

	output->SetForecolor(BLUE);
	output->Append("Compiling for profiling (with GHC)\n");
	output->SetForecolor(BLUE);

	LPTSTR x = strdup(recentFiles->Last());
	LPTSTR c = (x[0] == '\"' ? &x[1] : x);
	LPTSTR b = strrchr(c, '\\');
	if (b != NULL) b[0] = 0;
	SetCurrentDirectory(c);
	free(x);

	TCHAR Buffer[500];

	wsprintf(Buffer, "ghc -prof -auto-all --make %s", recentFiles->Last());

	Profile* p = new Profile();
	p->State = 0;
	p->Arguments = strdup(Arguments);
	p->Start(Buffer);
	return true;
}

void ProfileView()
{
}
