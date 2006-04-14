

class Input
{
public:
    Input(HWND hParent);
    ~Input();

    void Get(LPTSTR Buffer);
    void Set(LPCTSTR Buffer);
    void SelAll();
    void Sel(int Start, int Length);

//private:
    HWND hWnd;
    HWND hRTF;
};
