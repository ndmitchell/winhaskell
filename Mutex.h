
class Mutex
{
public:
    Mutex();
    ~Mutex();

    void Lock();

    //Lock without waiting
    // true = success (locked)
    // false = no change, is locked by someone else
    bool LockImmediate();

    void Unlock();

private:
    HANDLE hSemaphore;
};

class Lock
{
public:
    Lock(Mutex* m2){m = m2; m->Lock();};
    ~Lock(){m->Unlock();}

private:
    Mutex* m;
};
