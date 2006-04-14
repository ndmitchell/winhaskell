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

bool Mutex::LockImmediate()
{
    return (WaitForSingleObject(hSemaphore, 0) == WAIT_TIMEOUT);
}

void Mutex::Unlock()
{
	ReleaseSemaphore(hSemaphore, 1, NULL);
}
