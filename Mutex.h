
class Mutex
{
public:
    Mutex();
    ~Mutex();

    void Lock();
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
