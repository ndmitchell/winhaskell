#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "NullEval.h"
#include "Application.h"
#include "Output.h"
#include "Actions.h"

bool NullEval::Expression(LPCTSTR Expression)
{
    output->AppendLex(Expression);
    output->Append("\n");
	app->Warning("The NULL Interpreter cannot execute anything, install Hugs");
	return false;
}

bool NullEval::Execute(Action* a)
{
	return Expression(a->Orig);
}

NullEval* StartNullEval()
{
	return new NullEval();
}


