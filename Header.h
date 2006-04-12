#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <richedit.h>
#include <commctrl.h>
#include <tchar.h>
#include "resource.h"

#include <assert.h>


#define BLACK	    RGB(0,0,0)
#define BLUE	    RGB(0,0,175)
#define GREEN	    RGB(0,135,0)
#define CYAN	    RGB(0,128,128)
#define RED		    RGB(175,0,0)
#define BRIGHTRED   RGB(255,0,0)
#define MAGENTA	    RGB(150,0,150)
#define YELLOW	    RGB(175,175,0)
#define WHITE	    RGB(255,255,255)


void MainDialogFireCommand();
void SetStatusBar(LPCTSTR Text);



// Globally shared variables

extern HINSTANCE hInst;


const int ID_TOOLBAR = 6055;
const int ID_STATUS = 6056;
const int ID_EXPAND = 6057;
const int ID_ENTER = 6058;

int inline TwipToPoint(int x){return x / 20;}
int inline PointToTwip(int x){return x * 20;}





// From MainDialog

enum es

{

	esCompiling,

	esRunning,

	esFinished

};



void ShowMainDialog();

void ExecuteStage(es Stage);

void ExecutionComplete();


// From Execute

void Execute(LPCTSTR Command);



class Application;
class Toolbar;
class Interpreter;
class Compiler;
class Input;
class Output;
class RecentFiles;
class History;

enum Command;

extern Application* app;
extern Toolbar* toolbar;
extern Interpreter* interpreter;
extern Compiler* compiler;
extern Input* input;
extern Output* output;
extern RecentFiles* recentFiles;
extern History* history;

