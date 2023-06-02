#ifndef TABLE_H_230424_
#define TABLE_H_230424_

#include "../ResponseBase.h"

namespace fxdaemon
{
  class Table : public ResponseBase
  {
  public:
    typedef enum
    {
      ADDED = 0,
      CHANGED = 1,
      DELETED = 2,
    } Status;

    Table(Status status) : m_Status(status) {};
    void setStatus(Status status) { m_Status = status; };
    Status getStatus() const { return m_Status; };
    Napi::Value toValue(const Napi::Env &env) { return env.Null(); };

  protected:
    Status m_Status;
  };
}

#endif
