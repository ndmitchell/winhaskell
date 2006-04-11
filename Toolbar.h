
struct ToolbarTab
{
    bool HasRunStop;
    HWND hToolbar;
};

class Toolbar
{
public:
    Toolbar(HWND hParent);
    ~Toolbar();

    void RunningChanged(bool Running);
    int Height(){return 80;}

//private:
    HWND hWnd;
    HWND hTab;
    HIMAGELIST hNormalIml;
    HIMAGELIST hDisableIml;

    ToolbarTab Tabs[20];
    int TabCount;
};
