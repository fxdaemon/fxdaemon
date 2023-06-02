#ifndef TABLELISTENER_H_230423_
#define TABLELISTENER_H_230423_

#include "ForexConnect.h"
#include "AddRefWrapper.h"
#include "table/Account.h"
#include "table/Offer.h"
#include "table/Candle.h"
#include "table/Order.h"
#include "table/Trade.h"
#include "table/Summary.h"
#include "table/Message.h"

class TableListener : public AddRefWrapper<IO2GTableListener>
{
public:
  TableListener(EventQueue *eventQueue);
  virtual ~TableListener(){};

  virtual void onStatusChanged(O2GTableStatus);
  virtual void onAdded(const char *, IO2GRow *);
  virtual void onChanged(const char *, IO2GRow *);
  virtual void onDeleted(const char *, IO2GRow *);

  void subscribeEvents(IO2GTableManager *manager);
  void unsubscribeEvents(IO2GTableManager *manager);
  void onTableRow(IO2GRow *row, fxdaemon::Table::Status status);

  static std::shared_ptr<fxdaemon::Account> valueOfRow(
    IO2GAccountRow *accountRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Account> valueOfRow(
    IO2GAccountTableRow *accountRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Offer> valueOfRow(
    IO2GOfferRow *offerRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Offer> valueOfRow(
    IO2GOfferTableRow *offerRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Order> valueOfRow(
    IO2GOrderRow *orderRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Order> valueOfRow(
    IO2GOrderTableRow *orderRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Trade> valueOfRow(
    IO2GTradeRow *tradeRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Trade> valueOfRow(
    IO2GTradeTableRow *tradeRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::ClosedTrade> valueOfRow(
    IO2GClosedTradeRow *tradeRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::ClosedTrade> valueOfRow(
    IO2GClosedTradeTableRow *tradeRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Summary> valueOfRow(
    IO2GSummaryTableRow *summaryRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);
  static std::shared_ptr<fxdaemon::Message> valueOfRow(
    IO2GMessageRow *messageRow, fxdaemon::Table::Status status = fxdaemon::Table::ADDED);

private:
  EventQueue *m_EventQueue;
  bool m_Subscribed;
};

#endif