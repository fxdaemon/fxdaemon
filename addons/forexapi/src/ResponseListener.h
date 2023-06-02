#ifndef RESPONSELISTENER_H_230422_
#define RESPONSELISTENER_H_230422_

#include "ForexConnect.h"
#include "AddRefWrapper.h"
#include "TableListener.h"
#include "PriceCollector.h"

using MarketDataReaderCallback = std::function<void(IO2GMarketDataSnapshotResponseReader *)>;
using OffersTableReaderCallback = std::function<void(IO2GOffersTableResponseReader *)>;
using AccountsTableReaderCallback = std::function<void(IO2GAccountsTableResponseReader *)>;
using OrdersTableReaderCallback = std::function<void(IO2GOrdersTableResponseReader *)>;
using OpenTradesTableReaderCallback = std::function<void(IO2GTradesTableResponseReader *)>;
using ClosedTradesTableReaderCallback = std::function<void(IO2GClosedTradesTableResponseReader *)>;

class ResponseListener : public AddRefWrapper<IO2GResponseListener>
{
public:
  typedef enum
  {
    UNAVAILABLE = -1,
    COMPLETED = 0,
    FAILED = 1,
  } ResponseStatus;

  ResponseListener(IO2GSession *session, PriceCollector *priceCollector, EventQueue *eventQueue);
  virtual ~ResponseListener();

  void setRequestID(const char *requestID);
  void setRequestID(std::vector<std::string> &requestIDs);
  bool waitEvents();
  void reset();

  bool isCompleted() const { return m_Status == COMPLETED; };
  const std::string &getError() const { return m_Error; };
  std::string getOrderID();
  void getMarketDataSnapshot(MarketDataReaderCallback callback);
  void getRefreshTable(OffersTableReaderCallback callback);
  void getRefreshTable(AccountsTableReaderCallback callback);
  void getRefreshTable(OrdersTableReaderCallback callback);
  void getRefreshTable(OpenTradesTableReaderCallback callback);
  void getRefreshTable(ClosedTradesTableReaderCallback callback);

  virtual void onRequestCompleted(const char *requestID, IO2GResponse *response = 0);
  virtual void onRequestFailed(const char *requestID, const char *error);
  virtual void onTablesUpdates(IO2GResponse *data);

private:
  HANDLE m_Event;
  std::vector<std::string> m_RequestIDs;
  ResponseStatus m_Status;
  size_t m_CompletedCount;
  std::string m_Error;

  IO2GSession *m_Session;
  PriceCollector *m_PriceCollector;
  EventQueue *m_EventQueue;
  IO2GResponse *m_Response;
};

#endif
