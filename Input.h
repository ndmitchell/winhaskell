
void InputInit(HWND hInput);
void InputGet(LPTSTR Buffer);
void InputSet(LPCTSTR Buffer);
BOOL InputNotify(NMHDR* nmhdr);
void InputChanged();

const MaxInputSize = 500;
