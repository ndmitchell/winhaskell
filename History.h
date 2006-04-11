
const int HistoryMax = 25;

class History
{
public:
    History();
    ~History();

    void Add(LPCTSTR Item);
    LPCTSTR Get(int Delta);

private:
    // the oldest history is at the lowest number
    LPSTR Items[HistoryMax];
    int Pos;
    int Count;
};
