#ifndef MESSAGE_H_230510_
#define MESSAGE_H_230510_

#include "Table.h"

namespace fxdaemon
{
  class Message : public Table
  {
  public:
    Message(Status status);
    const std::string &getMsgID() const;
    void setMsgID(const char *msgID);
    time_t getTime() const;
    void setTime(time_t time);
    const std::string &getFrom() const;
    void setFrom(const char *from);
    const std::string &getType() const;
    void setType(const char *type);
    const std::string &getFeature() const;
    void setFeature(const char *feature);
    const std::string &getText() const;
    void setText(const char *text);
    const std::string &getSubject() const;
    void setSubject(const char *subject);
    bool getHTMLFragmentFlag() const;
    void setHTMLFragmentFlag(bool htmlFragmentFlag);

    virtual  Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_MsgID;
    time_t m_Time;
    std::string m_From;
    std::string m_Type;
    std::string m_Feature;
    std::string m_Text;
    std::string m_Subject;
    bool m_HTMLFragmentFlag;
  };
}

#endif
