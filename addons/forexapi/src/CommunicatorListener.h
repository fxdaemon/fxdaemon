#ifndef COMMUNICATORLISTENER_H_230426_
#define COMMUNICATORLISTENER_H_230426_

#include "ForexConnect.h"
#include "AddRefWrapper.h"

class CommunicatorListener : public AddRefWrapper<pricehistorymgr::IPriceHistoryCommunicatorListener>
{
public:
  CommunicatorListener(EventQueue *eventQueue);
  virtual ~CommunicatorListener();

  bool waitEvents();
  bool isCompleted() const;

  pricehistorymgr::IPriceHistoryCommunicatorResponse *getResponse() const;
  void setRequest(pricehistorymgr::IPriceHistoryCommunicatorRequest *request);

  virtual void onRequestCompleted(pricehistorymgr::IPriceHistoryCommunicatorRequest *request,
                                  pricehistorymgr::IPriceHistoryCommunicatorResponse *response);
  virtual void onRequestFailed(pricehistorymgr::IPriceHistoryCommunicatorRequest *request,
                               pricehistorymgr::IError *error);
  virtual void onRequestCancelled(pricehistorymgr::IPriceHistoryCommunicatorRequest *request);

private:
  EventQueue *m_EventQueue;
  HANDLE m_Event;
  bool m_Completed;

  pricehistorymgr::IPriceHistoryCommunicatorRequest *m_Request;
  pricehistorymgr::IPriceHistoryCommunicatorResponse *m_Response;
};

#endif
