
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
    void Move(POINT pt);

    void Show();
    void Hide();

    void GetCurrent(LPTSTR Buffer);
    void SetCurrent(LPCTSTR Str);
    void SetCurrentDelta(int Delta);
};
