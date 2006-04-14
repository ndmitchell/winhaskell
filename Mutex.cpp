#include "Header.h"
#include "Mutex.h"

Mutex::Mutex()
{
	hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
}

Mutex::~Mutex()
{
	CloseHandle(hSemaphore);
}

void Mutex::Lock()
{
    WaitForSingleObject(hSemaphore, INFINITE);
}

void Mutex::Unlock()
{
	ReleaseSemaphore(hSemaphore, 1, NULL);
}
