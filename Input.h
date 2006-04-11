

class Input
{
public:
    Input(HWND hParent);
    ~Input();

    void Get(LPTSTR Buffer);
    void Set(LPCTSTR Buffer);
    void SelAll();

//private:
    HWND hWnd;
    HWND hRTF;
};
