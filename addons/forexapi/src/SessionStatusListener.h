#ifndef SESSIONSTUTUSLISTENER_H_230422_
#define SESSIONSTUTUSLISTENER_H_230422_

#include "ForexConnect.h"
#include "AddRefWrapper.h"

class LoginDataProvider
{
public:
  LoginDataProvider(const char *sessionID, const char *pin)
      : m_SessionID(sessionID), m_Pin(pin){};
  const std::string &getSessionID() const { return m_SessionID; };
  const std::string &getPin() const { return m_Pin; };

private:
  std::string m_SessionID;
  std::string m_Pin;
};

class SessionStatusListener : public AddRefWrapper<IO2GSessionStatus>
{
public:
  SessionStatusListener(IO2GSession *session, LoginDataProvider *loginDataProvider, EventQueue *eventQueue);
  ~SessionStatusListener();

  bool waitEvents();
  bool isConnected() const { return m_Connected; };
  const std::string &getError() const { return m_Error; };
  
  virtual void onSessionStatusChanged(O2GSessionStatus status);
  virtual void onLoginFailed(const char *error);

private:
  bool m_Connected;
  HANDLE m_Event;
  std::string m_Error;

  IO2GSession *m_Session;
  LoginDataProvider *m_LoginDataProvider;
  EventQueue *m_EventQueue;
};

#endif
