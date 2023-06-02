#ifndef ADDREFWRAPPER_H_230505_
#define ADDREFWRAPPER_H_230505_

template <class T>
class AddRefWrapper : public T
{
public:
    AddRefWrapper() { m_Ref = 1; }
    virtual ~AddRefWrapper(){};

    virtual long addRef() { return InterlockedIncrement(&m_Ref); }
    virtual long release()
    {
        long ref = InterlockedDecrement(&m_Ref);
        if (ref == 0)  delete this;
        return ref;
    }

private:
    volatile long m_Ref;
};

#endif
