/*
* Copyright 2023 fxdaemon.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "stdafx.h"
#include "common/EventQueue.h"
#include "common/Utils.h"
#include "table/Offer.h"
#include "table/Candle.h"
#include "PriceCollector.h"

static const int DefaultTradingDayOffset = -7;
static const int PriceHistoryCount = 480;
static const double HalfSecond = 0.5 / 86400.0;
static const char *QuotesManagerDataPath = "historical_prices";

PriceCollector::PriceCollector(IO2GSession *session, EventQueue *eventQueue)
  : m_Session(session), m_EventQueue(eventQueue)
{
  m_RunState.store(UNFILL);
  m_TradingDayOffset = DefaultTradingDayOffset;
}

bool PriceCollector::init(std::string &error)
{
  pricehistorymgr::IError *_error = NULL;
  m_PriceHistoryCommunicator = 
    pricehistorymgr::PriceHistoryCommunicatorFactory::createCommunicator(m_Session, QuotesManagerDataPath, &_error);
  if (!m_PriceHistoryCommunicator) {
    error = _error->getMessage();
    _error->release();
    return false;
  }
  return true;
}

bool PriceCollector::addCommunicatorListener()
{
  if (!m_PriceHistoryCommunicator) {
    return false;
  }

  m_CommunicatorStatusListener = new CommunicatorStatusListener(m_EventQueue);
  m_PriceHistoryCommunicator->addStatusListener(m_CommunicatorStatusListener);
  if (!m_CommunicatorStatusListener->waitEvents() || !m_CommunicatorStatusListener->isReady()) {
    return false;
  }

  m_CommunicatorListener = new CommunicatorListener(m_EventQueue);
  m_PriceHistoryCommunicator->addListener(m_CommunicatorListener);
  if (!m_PriceHistoryCommunicator->isReady()) {
    m_EventQueue->push(Event::ERR, "History communicator is not ready.");
    m_PriceHistoryCommunicator->removeListener(m_CommunicatorListener);
    return false;
  }

  return true;
}

void PriceCollector::removeCommunicatorListener()
{
  if (m_PriceHistoryCommunicator) {
    if (m_CommunicatorListener) {
      m_PriceHistoryCommunicator->removeListener(m_CommunicatorListener);
      m_CommunicatorListener->release();
      m_CommunicatorListener = NULL;
    }
    if (m_CommunicatorStatusListener) {
      m_PriceHistoryCommunicator->removeStatusListener(m_CommunicatorStatusListener);
      m_CommunicatorStatusListener->release();
      m_CommunicatorStatusListener = NULL;
    }
    m_PriceHistoryCommunicator->release();
    m_PriceHistoryCommunicator = NULL;
  }
}

bool PriceCollector::renderLivePrices(const PeriodsType &periods, int priceHistoryCount)
{  
  if (!m_CommunicatorStatusListener || !m_CommunicatorListener) {
    m_RunState.store(PriceCollector::FAILED);
    return false;
  }

  priceHistoryCount = priceHistoryCount > 0 ? priceHistoryCount : PriceHistoryCount;
  for (const auto &entry : m_Offers) {
    for (const std::string &period : periods) {
      if (getHistoricalPrices(entry.first, period, 0, 0, priceHistoryCount, m_CommunicatorListener)) {
        m_Periods.insert(period);
      }
    }
  }
  if (m_Periods.size() > 0) {
    m_RunState.store(PriceCollector::READY);
  } else {
    m_RunState.store(PriceCollector::FAILED);
  }
  return m_Periods.size() > 0;
}

void PriceCollector::clearData()
{
  m_Offers.clear();
  m_CandlesSet.clear();
  m_PendingOffers.clear();
  m_Periods.clear();
}

void PriceCollector::setTradingDayOffset(const char *endTradingDayUTC)
{
  if (!endTradingDayUTC) {
    return;
  }

  m_EndTradingDay = endTradingDayUTC;  // 21:00:00
  int times[3] = {-1, -1, -1};  // hour, minutes, seconds
  size_t prev = 0;
  for (unsigned int i = 0; i < sizeof(times); i++) {
		size_t found = m_EndTradingDay.find_first_of(":", prev);
		if (found == std::string::npos) {
			times[i] = std::atoi(std::string(m_EndTradingDay, prev).c_str());
			break;
		}
		times[i] = std::atoi(std::string(m_EndTradingDay, prev, found - prev).c_str());
		prev = found + 1;
	}
  if (times[0] < 0 || times[0] > 23 || times[1] != 0 || times[2] != 0) {
    return;
  }

  tm st;
  Utils::time2Tm(0, &st);
  st.tm_hour = times[0];
  DATE date = Utils::tm2OleTime(&st);
  DATE estDate = utc2Est(date);
  Utils::oleTime2Tm(estDate, &st);
  if (st.tm_hour <= 12) {
    m_TradingDayOffset = st.tm_hour;
  } else {
    m_TradingDayOffset = st.tm_hour - 24;
  }
}

void PriceCollector::setRunState(RunState state)
{
  m_RunState.store(state);
}

bool PriceCollector::isCollectable(const char *symbol) const
{
  return m_RunState.load() > UNFILL && m_Offers.find(symbol) != m_Offers.end();
}

const OffersType &PriceCollector::getOffers() const
{
  return m_Offers;
}

std::shared_ptr<fxdaemon::Offer> PriceCollector::getOffer(const std::string &symbol) const
{
  OffersType::const_iterator it = m_Offers.find(symbol);
  if (it == m_Offers.end()) {
    return std::shared_ptr<fxdaemon::Offer>(nullptr);
  }
  return it->second;
}

std::shared_ptr<CandlesType> PriceCollector::getOrCreateCandles(const std::string &symbol,
                                                                const std::string &period)
{
  std::string key = symbol + period;
  CandlesSetType::const_iterator mpos = m_CandlesSet.find(key);
  if (mpos != m_CandlesSet.end()) {
    return mpos->second;
  }
  std::shared_ptr<CandlesType> candles = std::make_shared<CandlesType>();
  m_CandlesSet[key] = candles;
  return candles;
}

void PriceCollector::valueOf(const Napi::Env &env, Napi::Object &obj) const
{
  Napi::Array offers = Napi::Array::New(env, m_Offers.size());
  uint32_t i = 0;
  for (const auto &entry : m_Offers) {
    offers[i] = entry.second->toValue(env);
    i++;
  }
  obj.Set(Napi::String::New(env, "offers"), offers);

  Napi::Object candles = Napi::Object::New(env);
  for (const auto &entry : m_CandlesSet) {
    Napi::Array subCandles = Napi::Array::New(env, entry.second->size());
    for (uint32_t j = 0; j < subCandles.Length(); j++) {
      subCandles[j] = entry.second->at(j)->toValue(env);
    }
    candles.Set(Napi::String::New(env, entry.first), subCandles);
  }
  obj.Set(Napi::String::New(env, "candles"), candles);
}

void PriceCollector::updateOffer(std::shared_ptr<fxdaemon::Offer> offer)
{
  OffersType::iterator it = m_Offers.find(offer->getSymbol());
  if (it == m_Offers.end()) {
    m_Offers[offer->getSymbol()] = offer;
  } else {
    it->second->update(offer.get());
  }
}

void PriceCollector::onOfferUpdate(std::shared_ptr<fxdaemon::Offer> offer)
{
  m_EventQueue->push(Event::OFFER, offer);
  switch (m_RunState.load()) {
  case FILLED:
    m_PendingOffers.push_back(offer);
    break;
  case READY:
    if (!m_PendingOffers.empty()) {
      for (const std::shared_ptr<fxdaemon::Offer> &_offer : m_PendingOffers) {
        handleOffer(_offer);
        updateOffer(_offer);
      }
      m_PendingOffers.clear();
    }
    handleOffer(offer);
    updateOffer(offer);
    break;
  case FAILED:
    break;
  default:
    break;
  }
}

DATE PriceCollector::est2Utc(DATE date) const
{
  O2G2Ptr<IO2GTimeConverter> timeConverter = m_Session->getTimeConverter();
  return timeConverter->convert(date, IO2GTimeConverter::EST, IO2GTimeConverter::UTC);
}

DATE PriceCollector::utc2Est(DATE date) const
{
  O2G2Ptr<IO2GTimeConverter> timeConverter = m_Session->getTimeConverter();
  return timeConverter->convert(date, IO2GTimeConverter::UTC, IO2GTimeConverter::EST);
}

void PriceCollector::handleOffer(std::shared_ptr<fxdaemon::Offer> offer)
{
  for (const std::string &period : m_Periods) {
    std::shared_ptr<CandlesType> candles = getOrCreateCandles(offer->getSymbol(), period);
    quotesmgr::CandlePeriod::Unit timeframeUnit;
    int timeframeLength;
    if (quotesmgr::CandlePeriod::parsePeriod(period.c_str(), timeframeUnit, timeframeLength)) {
      handleOffer(offer, period, timeframeUnit, timeframeLength, candles);
    }
  }
}

void PriceCollector::handleOffer(std::shared_ptr<fxdaemon::Offer> offer, const std::string &period,
                                 quotesmgr::CandlePeriod::Unit timeframeUnit, int timeframeLength,
                                 std::shared_ptr<CandlesType> candles)
{
  const std::string &symbol = offer->getSymbol();
  std::shared_ptr<fxdaemon::Offer> prevOffer = getOffer(symbol);

  DATE start = -1;
  DATE end = -1;
  DATE curOleTime = Utils::time2OleTime(offer->getTime());
  quotesmgr::CandlePeriod::getCandle(utc2Est(curOleTime), start, end, timeframeUnit,
                                     timeframeLength, m_TradingDayOffset, -1);
  start = est2Utc(start);
  time_t startTime = Utils::oleTime2Time(start);
  INT64 curMinute = Utils::oleTime2Minute(curOleTime);
  INT64 prevMinute = Utils::oleTime2Minute(Utils::time2OleTime(prevOffer->getTime()));

  if (candles->size() == 0) {
    std::shared_ptr<fxdaemon::Candle> candle = std::make_shared<fxdaemon::Candle>(
        symbol.c_str(), period.c_str(), startTime,
        offer->getAsk(), offer->getBid(), offer->getVolume());
    candles->push_back(candle);
  } else  {
    std::shared_ptr<fxdaemon::Candle> lastCandle = candles->at(candles->size() - 1);
    DATE lastOleTime = Utils::time2OleTime(lastCandle->getStartTime());
    if (fabs(lastOleTime - start) < HalfSecond) {
      lastCandle->setAskClose(offer->getAsk());
      lastCandle->setBidClose(offer->getBid());
      lastCandle->setAskHighLow(offer->getAsk());
      lastCandle->setBidHighLow(offer->getBid());
      if (prevMinute == curMinute) {
        lastCandle->setVolume((lastCandle->getVolume() - prevOffer->getVolume()) + offer->getVolume());
      } else if (curMinute > prevMinute) {
        lastCandle->setVolume(lastCandle->getVolume() + offer->getVolume());
      }
    } else if (lastOleTime < start - HalfSecond) {
      m_EventQueue->push(Event::CANDLE, lastCandle);
      std::shared_ptr<fxdaemon::Candle> newCandle = std::make_shared<fxdaemon::Candle>(
          symbol.c_str(), period.c_str(), startTime,
          lastCandle->getAskClose(), lastCandle->getBidClose(), offer->getVolume());
      candles->push_back(newCandle);
      newCandle->setAskClose(offer->getAsk());
      newCandle->setBidClose(offer->getBid());
      newCandle->setAskHighLow(offer->getAsk());
      newCandle->setBidHighLow(offer->getBid());
    }
  }
}

bool PriceCollector::getHistoricalPrices(const std::string &symbol, const std::string &period,
                                         DATE from, DATE to, int count, CommunicatorListener *communicatorListener)
{
  pricehistorymgr::IError *error = NULL;
  auto handleError = [&](const char* _error) -> bool {
    m_EventQueue->push(Event::ERR, _error);
    if (error) error->release();
    return false;
  };

  O2G2Ptr<pricehistorymgr::ITimeframeFactory> timeframeFactory = m_PriceHistoryCommunicator->getTimeframeFactory();
  O2G2Ptr<IO2GTimeframe> timeFrame = timeframeFactory->create(period.c_str(), &error);
  if (!timeFrame) {
    return handleError(error->getMessage());
  }
  if (timeFrame->getUnit() == ::Tick) {
    return handleError("Cannot get historical prices with tick.");
  }

  O2G2Ptr<pricehistorymgr::IPriceHistoryCommunicatorRequest> request =
        m_PriceHistoryCommunicator->createRequest(symbol.c_str(), timeFrame, from, to, count, &error);
  if (!request) {
    return handleError(error->getMessage());
  }
  communicatorListener->setRequest(request);
  if (!m_PriceHistoryCommunicator->sendRequest(request, &error)) {
    return handleError(error->getMessage());
  }
  if (!communicatorListener->waitEvents() || !communicatorListener->isCompleted()) {
    return handleError("Cannot get historical prices.");
  }

  pricehistorymgr::IPriceHistoryCommunicatorResponse *response = communicatorListener->getResponse();
  O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = m_PriceHistoryCommunicator->createResponseReader(response, &error);
  if (error) {
    return handleError(error->getMessage());
  }

  std::shared_ptr<CandlesType> candles = getOrCreateCandles(symbol, period);
  for (int i = 0; i < reader->size(); ++i) {
    if (reader->isBar()) {
      std::shared_ptr<fxdaemon::Candle> candle = std::make_shared<fxdaemon::Candle>(
          symbol.c_str(), period.c_str(), Utils::oleTime2Time(reader->getDate(i)),
          reader->getAskOpen(i), reader->getAskHigh(i), reader->getAskLow(i), reader->getAskClose(i),
          reader->getBidOpen(i), reader->getBidHigh(i), reader->getBidLow(i), reader->getBidClose(i),
          reader->getVolume(i));
      candles->push_back(candle);
    } else {
      std::shared_ptr<fxdaemon::Candle> candle = std::make_shared<fxdaemon::Candle>(
          symbol.c_str(), period.c_str(), Utils::oleTime2Time(reader->getDate(i)),
          reader->getAsk(i), reader->getBid(i), reader->getVolume(i));
      candles->push_back(candle);
    }
  }

  std::string range;
  if (candles->size() > 0) {
    range = Utils::time2String(candles->at(0)->getStartTime()) + " - "
          + Utils::time2String(candles->at(candles->size() - 1)->getStartTime());
  }
  m_EventQueue->push(Event::INFO, "Historical prices [%s, %s] %s %d retrieved.",
      symbol.c_str(), period.c_str(), range.c_str(), reader->size());
  return true;
}
