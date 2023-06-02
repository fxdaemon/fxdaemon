#ifndef PRICECOLLECTOR_H_230427_
#define PRICECOLLECTOR_H_230427_

#include "CommunicatorStatusListener.h"
#include "CommunicatorListener.h"
#include "TableListener.h"

using SymbolsType = std::set<std::string>;
using PeriodsType = std::set<std::string>;
using OffersType = std::map<std::string, std::shared_ptr<fxdaemon::Offer>>;
using CandlesType = std::vector<std::shared_ptr<fxdaemon::Candle>>;
using CandlesSetType = std::map<std::string, std::shared_ptr<CandlesType>>;
using PendingOffersType = std::vector<std::shared_ptr<fxdaemon::Offer>>;

class PriceCollector
{
public:
  typedef enum
  {
    UNFILL = 0,
    FILLED = 1,
    READY = 2,
    FAILED = 3,
  } RunState;

  PriceCollector(IO2GSession *session, EventQueue *eventQueue);
  ~PriceCollector(){};

  bool init(std::string &error);
  bool addCommunicatorListener();
  void removeCommunicatorListener();
  void clearData();

  const std::string &getEndTradingDay() const { return m_EndTradingDay; };
  void setTradingDayOffset(const char *endTradingDayUTC);
  void setRunState(RunState state);
  bool isCollectable(const char *symbol) const;
  bool renderLivePrices(const PeriodsType &periods, int priceHistoryCount);
  const OffersType &getOffers() const;
  std::shared_ptr<fxdaemon::Offer> getOffer(const std::string &symbol) const;
  std::shared_ptr<CandlesType> getOrCreateCandles(const std::string &symbol, const std::string &period);
  void valueOf(const Napi::Env &env, Napi::Object &obj) const;
  void updateOffer(std::shared_ptr<fxdaemon::Offer> offer);
  void onOfferUpdate(std::shared_ptr<fxdaemon::Offer> offer);
  DATE est2Utc(DATE date) const;
  DATE utc2Est(DATE date) const;

private:
  void handleOffer(std::shared_ptr<fxdaemon::Offer> offer);
  void handleOffer(std::shared_ptr<fxdaemon::Offer> offer, const std::string &period,
                   quotesmgr::CandlePeriod::Unit timeframeUnit, int timeframeLength,
                   std::shared_ptr<CandlesType> candles);
  bool getHistoricalPrices(const std::string &symbol, const std::string &period,
                           DATE from, DATE to, int count, CommunicatorListener *communicatorListener);

  IO2GSession *m_Session;
  EventQueue *m_EventQueue;
  std::atomic<RunState> m_RunState;
  std::string m_EndTradingDay;
  int m_TradingDayOffset;
  OffersType m_Offers;               // <"EUR/USD", Offer>
  CandlesSetType m_CandlesSet;       // <"EUR/USDm5", [Candle, ...]>
  PendingOffersType m_PendingOffers; // [Offer, ...]
  PeriodsType m_Periods;             // <"m5", "m15", ...>

  pricehistorymgr::IPriceHistoryCommunicator *m_PriceHistoryCommunicator;
  CommunicatorStatusListener *m_CommunicatorStatusListener;
  CommunicatorListener *m_CommunicatorListener;
};

#endif
