
typedef void (*CompletionCallback)(LPCTSTR Result);

class Completion
{
private:
    HWND hWnd, hLst;
    CompletionCallback Callback;

public:
    Completion(CompletionCallback Callback);
    ~Completion();

    void Add(LPCTSTR Str);
    void Move(POINT Pos){};

    void Show();
    void Hide();

    LPCTSTR GetCurrent(){return NULL;};
    void SetCurrent(LPCTSTR Str){};
};
