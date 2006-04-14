
class String
{
public:
    String();
    String(LPCTSTR Value);
    ~String();

    LPCTSTR Get(){return Buffer;};
    void Set(LPCTSTR Value);
    void Append(LPCTSTR Value);
    void Reset(){Buffer = NULL;};

private:
    void SetInternal(LPCTSTR Value);

    int BufSize;
    int Size;
    LPTSTR Buffer;
};
