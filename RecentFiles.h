
const int MruCount = 9;

class RecentFiles
{
public:
    RecentFiles();
    ~RecentFiles();

    HMENU GetMenu();
    LPCTSTR Get(int Id);
	LPCTSTR Last();
    void Add(LPCTSTR Item);

private:
    LPTSTR Items[MruCount+1];
    HMENU hMenu;
};
