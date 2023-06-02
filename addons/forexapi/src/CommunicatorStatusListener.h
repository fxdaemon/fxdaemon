#ifndef COMMUNICATORSTATUSLISTENER_H_230426_
#define COMMUNICATORSTATUSLISTENER_H_230426_

#include "ForexConnect.h"
#include "AddRefWrapper.h"

class CommunicatorStatusListener : public AddRefWrapper<pricehistorymgr::IPriceHistoryCommunicatorStatusListener>
{
public:
  CommunicatorStatusListener(EventQueue *eventQueue);
  virtual ~CommunicatorStatusListener();

  bool isReady() const;
  bool waitEvents();

protected:
  virtual void onCommunicatorStatusChanged(bool ready);
  virtual void onCommunicatorInitFailed(pricehistorymgr::IError *error);

private:
  bool m_Ready;
  EventQueue *m_EventQueue;
  HANDLE m_Event;
};

#endif
