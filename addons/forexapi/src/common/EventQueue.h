#ifndef QUEUE_H_230422_
#define QUEUE_H_230422_

#include "Mutex.h"
#include "../ResponseBase.h"

static const char *EventTypeTexts[] = {
  "Info",
  "Error",
  "Session",
  "Offer",
  "Candle",
  "Account",
  "Order",
  "Trade",
  "ClosedTrade",
  "Message",
  "Summary",
};

struct Event
{
  typedef enum
  {
    UNKNOWN = -1,
    INFO = 0,
    ERR = 1,
    SESSION_STATUS = 2,
    OFFER = 3,
    CANDLE = 4,
    ACCOUNT = 5,
    ORDER = 6,
    TRADE = 7,
    CLOSED_TRADE = 8,
    MESSAGE = 9,
    SUMMARY = 10,
  } EventType;

  EventType type;
  std::shared_ptr<ResponseBase> data;
  bool isAvailable() { return type != UNKNOWN; };
  const char *getTypeText() { return EventTypeTexts[static_cast<int>(type)]; };
};

class EventQueue
{
public:
  EventQueue();
  ~EventQueue();

  void push(Event mq);
  void push(Event::EventType type, std::shared_ptr<ResponseBase> data);
  void push(std::string info);
  void push(Event::EventType type, const char *format, ...);
  Event pop();
  void clear();
  bool waitEvents();

private:
  Mutex m_mutex;
  HANDLE m_Event;
  std::queue<Event> m_queue;
};

#endif
