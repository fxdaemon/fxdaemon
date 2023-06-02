#ifndef MUTEX_H_230422_
#define MUTEX_H_230422_

class Mutex
{
public:
  Mutex();
  ~Mutex();
  void lock();
  void unlock();

  class Lock
  {
  private:
    Mutex *mutex_;

  public:
    Lock(Mutex &m) : mutex_(&m) { mutex_->lock(); }
    Lock(Mutex *m) : mutex_(m) { mutex_->lock(); }
    ~Lock() { mutex_->unlock(); }
  };

private:
#ifdef WIN32
  CRITICAL_SECTION m_critSection;
#else
  pthread_mutex_t m_mutex;
#endif
};

#endif
