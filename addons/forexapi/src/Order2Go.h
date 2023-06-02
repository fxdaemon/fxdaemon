#ifndef ORDER2GO_H_230424_
#define ORDER2GO_H_230424_

#include "SessionStatusListener.h"
#include "ResponseListener.h"
#include "TableListener.h"
#include "PriceCollector.h"

using AccountsType = std::vector<std::shared_ptr<fxdaemon::Account>>;
using OrdersType = std::vector<std::shared_ptr<fxdaemon::Order>>;
using TradesType = std::vector<std::shared_ptr<fxdaemon::Trade>>;
using ClosedTradesType = std::vector<std::shared_ptr<fxdaemon::ClosedTrade>>;
using SummaryType = std::vector<std::shared_ptr<fxdaemon::Summary>>;

class Order2Go
{
public:
  Order2Go(){};
  ~Order2Go(){};

  bool init(const char *sessionID, const char *pin, std::string &error);
  bool login(const char *userName, const char *password, const char *host,
             const char *connection, std::string &error);
  bool logout();
  void close();

  void loadData(SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount);
  void loadData(SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount,
                const Napi::Function &callback);
  bool waitEvents();
  Event popEvent();

  Napi::Value valueOf(const Napi::Env &env) const;
  time_t getServerTime() const;
  const std::string &getEndTradingDay() const;
  int getHistoricalPrices(const std::string &symbol, const std::string &period, time_t startTime,
                          time_t endTime, CandlesType &candles, std::string &error);
  void getHistoricalPrices(std::string &symbol, std::string &period, time_t startTime,
                          time_t endTime, const Napi::Function &callback);
  bool openPosition(const std::string &accountID, const std::string &symbol, const std::string &bs,
                    int amount, double stop, double limit, std::string &orderID, std::string &error);
  bool changeStopLoss(const std::string &accountID, const std::string &tradeID,
                      double amount, double stop, std::string &requestID, std::string &error);
  bool changeTakeProfit(const std::string &accountID, const std::string &tradeID,
                        double amount, double limit, std::string &requestID, std::string &error);
  bool closePosition(const std::string &accountID, const std::string &tradeID,
                     double amount, std::string &requestID, std::string &error);
  bool closeAllPositions(const std::string &accountID, const std::string &symbol,
                         std::vector<std::string> &requestIDs, std::string &error);

private:
  void loadTableData(IO2GTableManager *tableManager, SymbolsType &symbols);
  void loadTableData(SymbolsType &symbols, IO2GLoginRules *loginRules);
  void clearData();

  const char *getOppositeBS(const char *bs);
  IO2GTradeTableRow *getOpenTrade(const std::string &accountID, const std::string &tradeID);
  std::shared_ptr<fxdaemon::Account> getAccount(const std::string &accountID);
  std::string getOrderID(const std::string &accountID, const std::string &tradeID,
                         const std::string orderType);
  int getOffers(IO2GTableManager *tableManager, SymbolsType &symbols);
  int getOffers(IO2GLoginRules *loginRules, SymbolsType &symbols);
  int getAccounts(IO2GTableManager *tableManager);
  int getAccounts(IO2GLoginRules *loginRules);
  int getOrders(IO2GTableManager *tableManager);
  int getOrders(IO2GLoginRules *loginRules);
  int getOpenTrades(IO2GTableManager *tableManager);
  int getOpenTrades(IO2GLoginRules *loginRules);
  int getClosedTrades(IO2GTableManager *tableManager);
  int getClosedTrades(IO2GLoginRules *loginRules);
  int getSummary(IO2GTableManager *tableManager);

  void onConnected();
  void logTimeFrames();
  void logSystemProperties(IO2GLoginRules *loginRules);
  void logPermissions(IO2GLoginRules *loginRules);
  void logSettings(IO2GLoginRules *loginRules);
  static std::string getPermissionStatusText(O2GPermissionStatus permissionStatus);

  EventQueue m_EventQueue;
  AccountsType m_Accounts;
  OrdersType m_Orders;
  TradesType m_Trades;
  ClosedTradesType m_ClosedTrades;
  SummaryType m_Summary;

  IO2GSession *m_Session;
  SessionStatusListener *m_SessionStatusListener;
  ResponseListener *m_ResponseListener;
  TableListener *m_TableListener;
  PriceCollector *m_PriceCollector;
};

class LoadDataWorker : public Napi::AsyncWorker
{
public:
  LoadDataWorker(const Napi::Function &callback, Order2Go *order2Go,
                 SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount);
  virtual ~LoadDataWorker(){};

  virtual void Execute();
  virtual void OnOK();

private:
  Order2Go *m_Order2Go;
  SymbolsType m_Symbols;
  PeriodsType m_Periods;
  int m_PriceHistoryCount;
};

class GetHistoricalPricesWorker : public Napi::AsyncWorker
{
public:
  GetHistoricalPricesWorker(const Napi::Function &callback, Order2Go *order2Go,
                            std::string &symbol, std::string &period, time_t startTime, time_t endTime);
  virtual ~GetHistoricalPricesWorker(){};

  virtual void Execute();
  virtual void OnOK();

private:
  Order2Go *m_Order2Go;
  std::string m_Symbol;
  std::string m_Period;
  time_t m_StartTime;
  time_t m_EndTime;
  CandlesType m_Candles;
  std::string m_Error;
};

#endif
