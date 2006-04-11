
void OutputTimer();


class Output
{
public:
    Output(HWND hParent);
    ~Output();

    void SetBold(bool Bold);
    void SetColor(int Color);

    void Append(LPCTSTR Text);

    //Anything in the buffer from now on is frozen
    //Cannot be undone
    //void Freeze();
    //Move back one character
    //void Rewind();

// private:
    HWND hWnd;
    HWND hRTF;
};
