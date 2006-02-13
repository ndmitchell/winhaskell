void RecentFilesInit();
HMENU RecentFilesMenu();

// Return NULL if the recent file is not in the list
LPCTSTR RecentFilesGet(int Id);
void RecentFilesAdd(LPCTSTR Item);
