
enum ConsoleEscape;

struct OutputFormat
{
    int ForeColor;
    int BackColor;
    bool Bold;
    bool Underline;
};

class Output
{
public:
    Output(HWND hParent);
    ~Output();

	// Standard output
    void Append(LPCTSTR Text);

	// Format buffering
	void FormatReset();
	void FormatGet(OutputFormat* of);
	void FormatSet(OutputFormat* of);

	// Format changing
	void Escape(ConsoleEscape Code);
    void SetBold(bool Bold);
	void SetUnderline(bool Underline);
    void SetForecolor(int Color);
	void SetBackcolor(int Color);

	// Support for the backspace character
    //Anything in the buffer from now on is frozen
    //Cannot be undone
	void Freeze(){};
    //Move back one character
	void Rewind(){};

	void SelEnd();

// private:
	void SetCharFormat(CHARFORMAT* cf, DWORD Mask);

	int Length;

    HWND hWnd;
    HWND hRTF;
};
