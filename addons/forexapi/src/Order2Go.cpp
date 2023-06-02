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
#include "Order2Go.h"

bool Order2Go::init(const char *sessionID, const char *pin, std::string &error)
{
  m_Session = CO2GTransport::createSession();
  m_Session->useTableManager(::Yes, NULL);

  m_SessionStatusListener = new SessionStatusListener(
    m_Session, new LoginDataProvider(sessionID, pin), &m_EventQueue);
  m_Session->subscribeSessionStatus(m_SessionStatusListener);

  m_PriceCollector = new PriceCollector(m_Session, &m_EventQueue);
  return m_PriceCollector->init(error);
}

bool Order2Go::login(const char *userName, const char *password, const char *host,
                     const char *connection, std::string &error)
{
  m_Session->login(userName, password, host, connection);
  bool connected = m_SessionStatusListener->waitEvents() && m_SessionStatusListener->isConnected();
  if (connected) {
    m_PriceCollector->addCommunicatorListener();
    onConnected();
    m_ResponseListener = new ResponseListener(m_Session, m_PriceCollector, &m_EventQueue);
    m_Session->subscribeResponse(m_ResponseListener);
  } else {
    error = m_SessionStatusListener->getError();
  }
  return connected;
}

bool Order2Go::logout()
{
  if (!m_SessionStatusListener->isConnected()) {
    return false;
  }
  m_TableListener->unsubscribeEvents(m_Session->getTableManager());
  m_Session->unsubscribeResponse(m_ResponseListener);
  m_PriceCollector->removeCommunicatorListener();
  m_Session->logout();
  return m_SessionStatusListener->waitEvents();
}

void Order2Go::close()
{
  clearData();
  if (m_TableListener) {
    m_TableListener->release();
    m_TableListener = NULL;
  }
  if (m_ResponseListener) {
    m_ResponseListener->release();
    m_ResponseListener = NULL;
  }
  delete m_PriceCollector;
  m_Session->unsubscribeSessionStatus(m_SessionStatusListener);
  m_SessionStatusListener->release();
  m_Session->release();
}

void Order2Go::loadData(SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount)
{
  O2G2Ptr<IO2GTableManager> tableManager = m_Session->getTableManager();
  loadTableData(tableManager, symbols);  
  m_PriceCollector->renderLivePrices(periods, priceHistoryCount);

  m_TableListener = new TableListener(&m_EventQueue);
  m_TableListener->subscribeEvents(tableManager);
}

void Order2Go::loadData(SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount,
                        const Napi::Function &callback)
{
  LoadDataWorker *worker = new LoadDataWorker(callback, this, symbols, periods, priceHistoryCount);
  worker->Queue();
}

bool Order2Go::waitEvents()
{
  return m_EventQueue.waitEvents();
}

Event Order2Go::popEvent()
{
  return m_EventQueue.pop();
}

Napi::Value Order2Go::valueOf(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);

  // offers, candles
  m_PriceCollector->valueOf(env, obj);

  // accounts
  Napi::Array accounts = Napi::Array::New(env, m_Accounts.size());
  for (size_t i = 0; i < m_Accounts.size(); i++) {
     accounts[i] = m_Accounts[i]->toValue(env);
  }
  obj.Set(Napi::String::New(env, "accounts"), accounts);

  // orders
  Napi::Array orders = Napi::Array::New(env, m_Orders.size());
  for (size_t i = 0; i < m_Orders.size(); i++) {
     orders[i] = m_Orders[i]->toValue(env);
  }
  obj.Set(Napi::String::New(env, "orders"), orders);

  // trades
  Napi::Array trades = Napi::Array::New(env, m_Trades.size());
  for (size_t i = 0; i < m_Trades.size(); i++) {
     trades[i] = m_Trades[i]->toValue(env);
  }
  obj.Set(Napi::String::New(env, "trades"), trades);

  // closed trades
  Napi::Array closedTrades = Napi::Array::New(env, m_ClosedTrades.size());
  for (size_t i = 0; i < m_ClosedTrades.size(); i++) {
     closedTrades[i] = m_ClosedTrades[i]->toValue(env);
  }
  obj.Set(Napi::String::New(env, "closed_trades"), closedTrades);

  // summary
  Napi::Array summary = Napi::Array::New(env, m_Summary.size());
  for (size_t i = 0; i < m_Summary.size(); i++) {
     summary[i] = m_Summary[i]->toValue(env);
  }
  obj.Set(Napi::String::New(env, "summary"), summary);

  return obj;
}

time_t Order2Go::getServerTime() const
{
  return Utils::oleTime2Time(m_Session->getServerTime());
}

const std::string &Order2Go::getEndTradingDay() const
{
  return m_PriceCollector->getEndTradingDay();
}

int Order2Go::getHistoricalPrices(const std::string &symbol, const std::string &period, time_t startTime,
                                  time_t endTime, CandlesType &candles, std::string &error)
{
  auto addPrices = [&](IO2GMarketDataSnapshotResponseReader *reader) {
    int from = reader->size() - 1;
    if (candles.size() > 0) {
      time_t lastTime = candles[candles.size() - 1]->getStartTime();
      while (from >= 0 && Utils::oleTime2Time(reader->getDate(from)) >= lastTime) from--;
    }
    if (from < 0) return;
    for (int i = from; i >= 0; i--) {
      if (reader->isBar()) {
        std::shared_ptr<fxdaemon::Candle> candle = std::make_shared<fxdaemon::Candle>(
          symbol.c_str(), period.c_str(), Utils::oleTime2Time(reader->getDate(i)),
          reader->getAskOpen(i), reader->getAskHigh(i), reader->getAskLow(i), reader->getAskClose(i),
          reader->getBidOpen(i), reader->getBidHigh(i), reader->getBidLow(i), reader->getBidClose(i),
          reader->getVolume(i));
        candles.push_back(candle);
      } else {
        std::shared_ptr<fxdaemon::Candle> candle = std::make_shared<fxdaemon::Candle>(
          symbol.c_str(), period.c_str(), Utils::oleTime2Time(reader->getDate(i)),
          reader->getAsk(i), reader->getBid(i), reader->getVolume(i));
        candles.push_back(candle);
      }
    }
  };

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }
	O2G2Ptr<IO2GTimeframeCollection> timeframes = factory->getTimeFrameCollection();
	O2G2Ptr<IO2GTimeframe> timeframe = timeframes->get(period.c_str());
  if (!timeframe) {
    error = "Timeframe is invalid.";
    return false;
  }
	O2G2Ptr<IO2GRequest> request = factory->createMarketDataSnapshotRequestInstrument(
      symbol.c_str(), timeframe, timeframe->getQueryDepth());
  DATE startDate = Utils::time2OleTime(startTime);
  DATE endDate = Utils::time2OleTime(endTime);
  do {
    factory->fillMarketDataSnapshotRequestTime(request, startDate, endDate, true);
    m_ResponseListener->setRequestID(request->getRequestID());
    m_Session->sendRequest(request);
    if (!m_ResponseListener->waitEvents()) {
      break;
    }
    bool exit = false;
    m_ResponseListener->getMarketDataSnapshot([&](IO2GMarketDataSnapshotResponseReader *reader) {
      if (!reader || reader->size() == 0) {
        exit = true;
        return;
      }
      m_EventQueue.push(Event::INFO, "Retrieved historical prices.  size:%d  [req] %s - %s  [res] %s - %s",
        reader->size(), Utils::oleTime2String(startDate).c_str(), Utils::oleTime2String(endDate).c_str(),
        Utils::oleTime2String(reader->getDate(0)).c_str(), Utils::oleTime2String(reader->getDate(reader->size() - 1)).c_str());
      addPrices(reader);
      if (fabs(endDate - reader->getDate(0)) > 0.0001) {
        endDate = reader->getDate(0);
      } else {
        exit = true;
      }
    });
    if (exit) break;
  } while (endDate - startDate > 0.0001);

  std::reverse(candles.begin(), candles.end());
  error = m_ResponseListener->getError();
  return candles.size();
}

void Order2Go::getHistoricalPrices(std::string &symbol, std::string &period, time_t startTime,
                                  time_t endTime, const Napi::Function &callback)
{
  GetHistoricalPricesWorker *worker = new GetHistoricalPricesWorker(
      callback, this, symbol, period, startTime, endTime);
  worker->Queue();
}

bool Order2Go::openPosition(const std::string &accountID, const std::string &symbol, const std::string &bs,
                            int amount, double stop, double limit, std::string &orderID, std::string &error)
{
  std::shared_ptr<fxdaemon::Account> account = getAccount(accountID);
  if (!account) {
    error = "The account does not exist.";
    return false;
  }
  std::shared_ptr<fxdaemon::Offer> offer = m_PriceCollector->getOffer(symbol);
  if (!offer) {
    error = "The symbol does not exist.";
    return false;
  }

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }
  O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
  valuemap->setString(::Command, O2G2::Commands::CreateOrder);
  valuemap->setString(::OrderType, O2G2::Orders::TrueMarketOpen);
  valuemap->setString(::AccountID, accountID.c_str());
  valuemap->setString(::OfferID, offer->getOfferID().c_str());
  valuemap->setString(::BuySell, bs.c_str());
  valuemap->setInt(::Amount, amount);
  if (stop > 0) {
    valuemap->setDouble(::RateStop, stop);
  }
  if (limit > 0) {
    valuemap->setDouble(::RateLimit, limit);
  }
  O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
  if (!request) {
    error = factory->getLastError();
    return false;
  }

  m_ResponseListener->setRequestID(request->getRequestID());
  m_Session->sendRequest(request);
  m_ResponseListener->waitEvents();
  orderID = m_ResponseListener->getOrderID();
  error = m_ResponseListener->getError();
  return m_ResponseListener->isCompleted();
}

bool Order2Go::changeStopLoss(const std::string &accountID, const std::string &tradeID,
                              double amount, double stop, std::string &requestID, std::string &error)
{
  O2G2Ptr<IO2GTradeTableRow> trade = getOpenTrade(accountID, tradeID);
  if (!trade) {
    error = "The position does not exist.";
    return false;
  }

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }
	O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
	std::string stopOrderID = getOrderID(accountID, tradeID, O2G2::Orders::Stop);
	if (stopOrderID.empty()) {
		valuemap->setString(::Command, O2G2::Commands::CreateOrder);
		valuemap->setString(::OrderType, O2G2::Orders::Stop);
		valuemap->setString(::AccountID, accountID.c_str());
		valuemap->setString(::OfferID, trade->getOfferID());
		valuemap->setString(::TradeID, tradeID.c_str());
		valuemap->setString(::BuySell, getOppositeBS(trade->getBuySell()));
		valuemap->setInt(::Amount, amount > 0 ? amount : trade->getAmount());
		valuemap->setDouble(::Rate, stop);
	} else {
		valuemap->setString(::Command, O2G2::Commands::EditOrder);
		valuemap->setString(::OrderID, stopOrderID.c_str());
		valuemap->setString(::AccountID, accountID.c_str());
		valuemap->setDouble(::Rate, stop);
	}
	O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
  if (!request) {
    error = factory->getLastError();
    return false;
  }

  m_ResponseListener->setRequestID(request->getRequestID());
  m_Session->sendRequest(request);
  m_ResponseListener->waitEvents();
  requestID = request->getRequestID();
  error = m_ResponseListener->getError();
  return m_ResponseListener->isCompleted();
}

bool Order2Go::changeTakeProfit(const std::string &accountID, const std::string &tradeID,
                                double amount, double limit, std::string &requestID, std::string &error)
{
  O2G2Ptr<IO2GTradeTableRow> trade = getOpenTrade(accountID, tradeID);
  if (!trade) {
    error = "The position does not exist.";
    return false;
  }

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }
	O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
	std::string limitOrderID = getOrderID(accountID, tradeID, O2G2::Orders::Limit);
	if (limitOrderID.empty()) {
		valuemap->setString(::Command, O2G2::Commands::CreateOrder);
    valuemap->setString(::OrderType, O2G2::Orders::Limit);
		valuemap->setString(::AccountID, accountID.c_str());
		valuemap->setString(::OfferID, trade->getOfferID());
		valuemap->setString(::TradeID, tradeID.c_str());
		valuemap->setString(::BuySell, getOppositeBS(trade->getBuySell()));
		valuemap->setInt(::Amount, amount > 0 ? amount : trade->getAmount());
		valuemap->setDouble(::Rate, limit);
	} else {
		valuemap->setString(::Command, O2G2::Commands::EditOrder);
		valuemap->setString(::OrderID, limitOrderID.c_str());
		valuemap->setString(::AccountID, accountID.c_str());
		valuemap->setDouble(::Rate, limit);
	}
	O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
  if (!request) {
    error = factory->getLastError();
    return false;
  }

  m_ResponseListener->setRequestID(request->getRequestID());
  m_Session->sendRequest(request);
  m_ResponseListener->waitEvents();
  requestID = request->getRequestID();
  error = m_ResponseListener->getError();
  return m_ResponseListener->isCompleted();
}

bool Order2Go::closePosition(const std::string &accountID, const std::string &tradeID,
                             double amount, std::string &requestID, std::string &error)
{
  O2G2Ptr<IO2GTradeTableRow> trade = getOpenTrade(accountID, tradeID);
  if (!trade) {
    error = "The position does not exist.";
    return false;
  }

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }
  O2G2Ptr<IO2GLoginRules> loginRules = m_Session->getLoginRules();
  O2G2Ptr<IO2GPermissionChecker> permissionChecker = loginRules->getPermissionChecker();
  O2G2Ptr<IO2GValueMap> valuemap = factory->createValueMap();
  valuemap->setString(::Command, O2G2::Commands::CreateOrder);
  if (permissionChecker->canCreateMarketCloseOrder(trade->getInstrument()) != ::PermissionEnabled) {
    valuemap->setString(::OrderType, O2G2::Orders::TrueMarketOpen);
  } else {
    valuemap->setString(::OrderType, O2G2::Orders::TrueMarketClose);
    valuemap->setString(::TradeID, trade->getTradeID());
  }
  valuemap->setString(::AccountID, trade->getAccountID());
  valuemap->setString(::OfferID, trade->getOfferID());
  valuemap->setString(::BuySell, getOppositeBS(trade->getBuySell()));
  valuemap->setInt(::Amount, amount > 0 ? amount : trade->getAmount());
  O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(valuemap);
  if (!request) {
    error = factory->getLastError();
    return false;
  }

  m_ResponseListener->setRequestID(request->getRequestID());
  m_Session->sendRequest(request);
  m_ResponseListener->waitEvents();
  requestID = request->getRequestID();
  error = m_ResponseListener->getError();
  return m_ResponseListener->isCompleted();
}

bool Order2Go::closeAllPositions(const std::string &accountID, const std::string &symbol,
                                 std::vector<std::string> &requestIDs, std::string &error)
{
  std::shared_ptr<fxdaemon::Account> account = getAccount(accountID);
  if (!account) {
    error = "The account does not exist.";
    return false;
  }

  std::string offerID;
  if (!symbol.empty()) {
    std::shared_ptr<fxdaemon::Offer> offer = m_PriceCollector->getOffer(symbol);
    if (!offer) {
      error = "The symbol does not exist.";
      return false;
    }
    offerID = offer->getOfferID();
  }

  std::map<std::string, std::string> offerBSInfo;
  O2G2Ptr<IO2GTableManager> tableManager = m_Session->getTableManager();
  O2G2Ptr<IO2GTradesTable> tradesTable = (IO2GTradesTable*)tableManager->getTable(::Trades);
  for (int i = 0; i < tradesTable->size(); i++) {
    O2G2Ptr<IO2GTradeTableRow> trade = tradesTable->getRow(i);
    if (accountID != trade->getAccountID() || (!offerID.empty() && offerID != trade->getOfferID())) {
      continue;
    }
    std::map<std::string, std::string>::iterator it = offerBSInfo.find(trade->getOfferID());
    if (it == offerBSInfo.end()) {
      offerBSInfo[trade->getOfferID()] = trade->getBuySell();
    } else {
      std::string::size_type pos = it->second.find(trade->getBuySell());
      if (pos == std::string::npos) {
        it->second.append(trade->getBuySell());
      }
    }
  }
  if (offerBSInfo.empty()) {
    return true;
  }

  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  if (!factory) {
    error = "Cannot create request factory.";
    return false;
  }

  auto newOrderValueMap = [&](const std::string &offerID, const std::string &bs) -> IO2GValueMap* {
    IO2GValueMap *orderValuemap = factory->createValueMap();
    orderValuemap->setString(::Command, O2G2::Commands::CreateOrder);
    orderValuemap->setString(::NetQuantity, "Y");
    orderValuemap->setString(::OrderType, O2G2::Orders::TrueMarketClose);
    orderValuemap->setString(::AccountID, accountID.c_str());
    orderValuemap->setString(::OfferID, offerID.c_str());
    orderValuemap->setString(::BuySell, getOppositeBS(bs.c_str()));
    return orderValuemap;
  };

  O2G2Ptr<IO2GValueMap> batchValuemap = factory->createValueMap();
  batchValuemap->setString(::Command, O2G2::Commands::CreateOrder);
  for (const auto &offerBS : offerBSInfo) {
    if (offerBS.second.size() == 1) {
      batchValuemap->appendChild(newOrderValueMap(offerBS.first, offerBS.second));
    } else if (offerBS.second.size() == 2) {
      batchValuemap->appendChild(newOrderValueMap(offerBS.first, offerBS.second.substr(0, 1)));
      batchValuemap->appendChild(newOrderValueMap(offerBS.first, offerBS.second.substr(1, 1)));
    }
  }
  O2G2Ptr<IO2GRequest> request = factory->createOrderRequest(batchValuemap);
  if (!request) {
    error = factory->getLastError();
    return false;
  }

  requestIDs.resize(request->getChildrenCount());
  for (int i = 0; i < request->getChildrenCount(); ++i) {
    O2G2Ptr<IO2GRequest> requestChild = request->getChildRequest(i);
    requestIDs[i] = requestChild->getRequestID();
  }
  m_ResponseListener->setRequestID(requestIDs);
  m_Session->sendRequest(request);
  m_ResponseListener->waitEvents();
  error = m_ResponseListener->getError();
  return m_ResponseListener->isCompleted();
}

void Order2Go::loadTableData(IO2GTableManager *tableManager, SymbolsType &symbols)
{
  while (tableManager->getStatus() == TablesLoading) {
    Sleep(50);
  }
  if (tableManager->getStatus() == TablesLoadFailed) {
    m_EventQueue.push(Event::ERR, "Cannot refresh all tables of table manager.");
  }

  getOffers(tableManager, symbols);
  getAccounts(tableManager);
  getOrders(tableManager);
  getOpenTrades(tableManager);
  getClosedTrades(tableManager);
  getSummary(tableManager);
}

// reserved
void Order2Go::loadTableData(SymbolsType &symbols, IO2GLoginRules *loginRules)
{
  getOffers(loginRules, symbols);
  getAccounts(loginRules);
  getOrders(loginRules);
  getOpenTrades(loginRules);
  getClosedTrades(loginRules);
}

void Order2Go::clearData()
{
  m_EventQueue.clear();
  m_Accounts.clear();
  m_Orders.clear();
  m_Trades.clear();
  m_ClosedTrades.clear();
  m_Summary.clear();
  m_PriceCollector->clearData();
}

const char *Order2Go::getOppositeBS(const char *bs)
{
  return strcmp(bs, O2G2::Buy) == 0 ? O2G2::Sell : O2G2::Buy;
}

IO2GTradeTableRow *Order2Go::getOpenTrade(const std::string &accountID, const std::string &tradeID)
{
  O2G2Ptr<IO2GTableManager> tableManager = m_Session->getTableManager();
  O2G2Ptr<IO2GTradesTable> tradesTable = (IO2GTradesTable*)tableManager->getTable(::Trades);
  for (int i = 0; i < tradesTable->size(); i++) {
    O2G2Ptr<IO2GTradeTableRow> trade = tradesTable->getRow(i);
    if (accountID == trade->getAccountID() && tradeID == trade->getTradeID()) {
      return trade.Detach();
    }
  }
  return NULL;
}

std::shared_ptr<fxdaemon::Account> Order2Go::getAccount(const std::string &accountID)
{
  for (const auto &account : m_Accounts) {
    if (account->getAccountID() == accountID) {
      return account;
    }
  }
  return std::shared_ptr<fxdaemon::Account>(nullptr);
}

std::string Order2Go::getOrderID(const std::string &accountID, const std::string &tradeID, const std::string orderType)
{
  O2G2Ptr<IO2GTableManager> tableManager = m_Session->getTableManager();
  O2G2Ptr<IO2GOrdersTable> ordersTable = (IO2GOrdersTable*)tableManager->getTable(::Orders);
  for (int i = 0; i < ordersTable->size(); i++) {
    O2G2Ptr<IO2GOrderTableRow> order = ordersTable->getRow(i);
    if (accountID == order->getAccountID() && tradeID == order->getTradeID() && orderType == order->getType()) {
      return order->getOrderID();
    }
  }
  return "";
}

int Order2Go::getOffers(IO2GTableManager *tableManager, SymbolsType &symbols)
{
  O2G2Ptr<IO2GOffersTable> offersTable = (IO2GOffersTable*)tableManager->getTable(::Offers);
  IO2GOfferTableRow *offerRow = NULL;
  IO2GTableIterator tableIterator;
  while (offersTable->getNextRow(tableIterator, offerRow)) {
    if (symbols.find(offerRow->getInstrument()) != symbols.end()) {
      std::shared_ptr<fxdaemon::Offer> offer = TableListener::valueOfRow(offerRow);
      m_PriceCollector->updateOffer(offer);
    }
    offerRow->release();
  }
  m_PriceCollector->setRunState(PriceCollector::FILLED);
  return m_PriceCollector->getOffers().size();
}

int Order2Go::getOffers(IO2GLoginRules *loginRules, SymbolsType &symbols)
{
  auto updateOffers = [&](IO2GOffersTableResponseReader *reader) {
    for (int i = 0; i < reader->size(); ++i) {
      O2G2Ptr<IO2GOfferRow> row = reader->getRow(i);
      if (symbols.find(row->getInstrument()) != symbols.end()) {
        m_PriceCollector->updateOffer(TableListener::valueOfRow(row));
      }
    }
  };

  if (loginRules && loginRules->isTableLoadedByDefault(::Offers)) {
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Offers);
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    O2G2Ptr<IO2GOffersTableResponseReader> reader = factory->createOffersTableReader(response);
    updateOffers(reader);
  } else {
    O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
    if (factory) {
      O2G2Ptr<IO2GRequest> request = factory->createRefreshTableRequest(::Offers);
      m_ResponseListener->setRequestID(request->getRequestID());
      m_Session->sendRequest(request);
      if (m_ResponseListener->waitEvents()) {
        m_ResponseListener->getRefreshTable([&](IO2GOffersTableResponseReader *reader) {
          updateOffers(reader);
        });
      } else {
        m_EventQueue.push(Event::ERR, "Cannot retrieve offers with RefreshTableRequest.");
      }
    }
  }
  m_PriceCollector->setRunState(PriceCollector::FILLED);
  return m_PriceCollector->getOffers().size();
}

int Order2Go::getAccounts(IO2GTableManager *tableManager)
{
  O2G2Ptr<IO2GAccountsTable> accountsTable = (IO2GAccountsTable*)tableManager->getTable(::Accounts);
  IO2GAccountTableRow *accountRow = NULL;
  IO2GTableIterator tableIterator;
  while (accountsTable->getNextRow(tableIterator, accountRow)) {
    std::shared_ptr<fxdaemon::Account> account = TableListener::valueOfRow(accountRow);
    m_Accounts.push_back(account);
    accountRow->release();
  }
  return m_Accounts.size();
}

int Order2Go::getAccounts(IO2GLoginRules *loginRules)
{
  auto addAccounts = [&](IO2GAccountsTableResponseReader *reader) {
    for (int i = 0; i < reader->size(); ++i) {
      O2G2Ptr<IO2GAccountRow> row = reader->getRow(i);
      std::shared_ptr<fxdaemon::Account> account = TableListener::valueOfRow(row);
      m_Accounts.push_back(account);
    }
  };

  if (loginRules && loginRules->isTableLoadedByDefault(::Accounts)) {
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Accounts);
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    O2G2Ptr<IO2GAccountsTableResponseReader> reader = factory->createAccountsTableReader(response);
    addAccounts(reader);
  } else {
    O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
    if (factory) {
      O2G2Ptr<IO2GRequest> request = factory->createRefreshTableRequest(::Accounts);
      m_ResponseListener->setRequestID(request->getRequestID());
      m_Session->sendRequest(request);
      if (m_ResponseListener->waitEvents()) {
        m_ResponseListener->getRefreshTable([&](IO2GAccountsTableResponseReader *reader) {
          addAccounts(reader);
        });
      } else {
        m_EventQueue.push(Event::ERR, "Cannot retrieve accounts with RefreshTableRequest.");
      }
    }
  }
  return m_Accounts.size();
}

int Order2Go::getOrders(IO2GTableManager *tableManager)
{
  O2G2Ptr<IO2GOrdersTable> ordersTable = (IO2GOrdersTable*)tableManager->getTable(::Orders);
  IO2GOrderTableRow *orderRow = NULL;
  IO2GTableIterator tableIterator;
  while (ordersTable->getNextRow(tableIterator, orderRow)) {
    std::shared_ptr<fxdaemon::Order> order = TableListener::valueOfRow(orderRow);
    m_Orders.push_back(order);
    orderRow->release();
  }
  return 0;
}

int Order2Go::getOrders(IO2GLoginRules *loginRules)
{
  auto addOrders = [&](IO2GOrdersTableResponseReader *reader) {
    for (int i = 0; i < reader->size(); ++i) {
      O2G2Ptr<IO2GOrderRow> row = reader->getRow(i);
      std::shared_ptr<fxdaemon::Order> order = TableListener::valueOfRow(row);
      m_Orders.push_back(order);
    }
  };

  if (loginRules && loginRules->isTableLoadedByDefault(::Orders)) {
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Orders);
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    O2G2Ptr<IO2GOrdersTableResponseReader> reader = factory->createOrdersTableReader(response);
    addOrders(reader);
  } else {
    O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
    if (factory) {
      O2G2Ptr<IO2GRequest> request = factory->createRefreshTableRequest(::Orders);
      m_ResponseListener->setRequestID(request->getRequestID());
      m_Session->sendRequest(request);
      if (m_ResponseListener->waitEvents()) {
        m_ResponseListener->getRefreshTable([&](IO2GOrdersTableResponseReader *reader) {
          addOrders(reader);
        });
      } else {
        m_EventQueue.push(Event::ERR, "Cannot retrieve orders with RefreshTableRequest.");
      }
    }
  }
  return m_Orders.size();
}

int Order2Go::getOpenTrades(IO2GTableManager *tableManager)
{
  O2G2Ptr<IO2GTradesTable> tradesTable = (IO2GTradesTable*)tableManager->getTable(::Trades);
  IO2GTradeTableRow *tradeRow = NULL;
  IO2GTableIterator tableIterator;
  while (tradesTable->getNextRow(tableIterator, tradeRow)) {
    std::shared_ptr<fxdaemon::Trade> trade = TableListener::valueOfRow(tradeRow);
    m_Trades.push_back(trade);
    tradeRow->release();
  }
  return 0;
}

int Order2Go::getOpenTrades(IO2GLoginRules *loginRules)
{
  auto addTrades = [&](IO2GTradesTableResponseReader *reader) {
    for (int i = 0; i < reader->size(); ++i) {
      O2G2Ptr<IO2GTradeRow> row = reader->getRow(i);
      std::shared_ptr<fxdaemon::Trade> trade = TableListener::valueOfRow(row);
      m_Trades.push_back(trade);
    }
  };

  if (loginRules && loginRules->isTableLoadedByDefault(::Trades)) {
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::Trades);
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    O2G2Ptr<IO2GTradesTableResponseReader> reader = factory->createTradesTableReader(response);
    addTrades(reader);
  } else {
    O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
    if (factory) {
      O2G2Ptr<IO2GRequest> request = factory->createRefreshTableRequest(::Trades);
      m_ResponseListener->setRequestID(request->getRequestID());
      m_Session->sendRequest(request);
      if (m_ResponseListener->waitEvents()) {
        m_ResponseListener->getRefreshTable([&](IO2GTradesTableResponseReader *reader) {
          addTrades(reader);
        });
      } else {
        m_EventQueue.push(Event::ERR, "Cannot retrieve trades with RefreshTableRequest.");
      }
    }
  }
  return m_Trades.size();
}

int Order2Go::getClosedTrades(IO2GTableManager *tableManager)
{
  O2G2Ptr<IO2GClosedTradesTable> tradesTable = (IO2GClosedTradesTable*)tableManager->getTable(::ClosedTrades);
  IO2GClosedTradeTableRow *tradeRow = NULL;
  IO2GTableIterator tableIterator;
  while (tradesTable->getNextRow(tableIterator, tradeRow)) {
    std::shared_ptr<fxdaemon::ClosedTrade> trade = TableListener::valueOfRow(tradeRow);
    m_ClosedTrades.push_back(trade);
    tradeRow->release();
  }
  return 0;
}

int Order2Go::getClosedTrades(IO2GLoginRules *loginRules)
{
  auto addClosedTrades = [&](IO2GClosedTradesTableResponseReader *reader) {
    for (int i = 0; i < reader->size(); ++i) {
      O2G2Ptr<IO2GClosedTradeRow> row = reader->getRow(i);
      std::shared_ptr<fxdaemon::ClosedTrade> closedTrade = TableListener::valueOfRow(row);
      m_ClosedTrades.push_back(closedTrade);
    }
  };

  if (loginRules && loginRules->isTableLoadedByDefault(::ClosedTrades)) {
    O2G2Ptr<IO2GResponse> response = loginRules->getTableRefreshResponse(::ClosedTrades);
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    O2G2Ptr<IO2GClosedTradesTableResponseReader> reader = factory->createClosedTradesTableReader(response);
    addClosedTrades(reader);
  } else {
    O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
    if (factory) {
      O2G2Ptr<IO2GRequest> request = factory->createRefreshTableRequest(::ClosedTrades);
      m_ResponseListener->setRequestID(request->getRequestID());
      m_Session->sendRequest(request);
      if (m_ResponseListener->waitEvents()) {
        m_ResponseListener->getRefreshTable([&](IO2GClosedTradesTableResponseReader *reader) {
          addClosedTrades(reader);
        });
      } else {
        m_EventQueue.push(Event::ERR, "Cannot retrieve closed trades with RefreshTableRequest.");
      }
    }
  }
  return m_ClosedTrades.size();
}

int Order2Go::getSummary(IO2GTableManager *tableManager)
{
  O2G2Ptr<IO2GSummaryTable> summaryTable = (IO2GSummaryTable*)tableManager->getTable(::Summary);
  IO2GSummaryTableRow *summaryRow = NULL;
  IO2GTableIterator tableIterator;
  while (summaryTable->getNextRow(tableIterator, summaryRow)) {
    std::shared_ptr<fxdaemon::Summary> summary = TableListener::valueOfRow(summaryRow);
    m_Summary.push_back(summary);
    summaryRow->release();
  }
  return 0;
}

void Order2Go::onConnected()
{
  logTimeFrames();
  O2G2Ptr<IO2GLoginRules> loginRules = m_Session->getLoginRules();
  if (loginRules) {
    logSystemProperties(loginRules);
    // logPermissions(loginRules);
    logSettings(loginRules);
  }
}

void Order2Go::logTimeFrames()
{
  std::string s = "[TimeFrames]";
  char szBuffer[256];
  O2G2Ptr<IO2GRequestFactory> factory = m_Session->getRequestFactory();
  O2G2Ptr<IO2GTimeframeCollection> timeFrames = factory->getTimeFrameCollection();
  for (int i = 0; i < timeFrames->size(); i++) {
    O2G2Ptr<IO2GTimeframe> timeFrame = timeFrames->get(i);
    sprintf_s(szBuffer, sizeof(szBuffer), "\nTime frame id=%s unit=%d size=%d", timeFrame->getID(), timeFrame->getUnit(), timeFrame->getSize());
    s += szBuffer;
  }
  m_EventQueue.push(std::move(s));
}

void Order2Go::logSystemProperties(IO2GLoginRules *loginRules)
{
  std::string s = "[System Properties]";
  O2G2Ptr<IO2GResponse> response = loginRules->getSystemPropertiesResponse();
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GSystemPropertiesReader> reader = factory->createSystemPropertiesReader(response);
    if (reader) {
      m_PriceCollector->setTradingDayOffset(reader->findProperty("END_TRADING_DAY"));
      for (int i = 0 ; i < reader->size(); i++) {
        const char *value;
        const char *sysProperty = reader->getProperty(i, value);
        s = s + "\n" + sysProperty + " = " + value;
      }
    }
  }
  m_EventQueue.push(std::move(s));
}

void Order2Go::logPermissions(IO2GLoginRules *loginRules)
{
  std::string s = "[Permissions]";
  O2G2Ptr<IO2GPermissionChecker> permissionChecker = loginRules->getPermissionChecker();
  for (const auto &entry : m_PriceCollector->getOffers()) {
    const std::string &symbol = entry.first;
    const char *instrument = symbol.c_str();
    s = s + "\n[" + symbol + "]";
    s = s + "\ncanCreateMarketOpenOrder: " + getPermissionStatusText(permissionChecker->canCreateMarketOpenOrder(instrument));
    s = s + "\ncanChangeMarketOpenOrder: " + getPermissionStatusText(permissionChecker->canChangeMarketOpenOrder(instrument));
    s = s + "\ncanDeleteMarketOpenOrder: " + getPermissionStatusText(permissionChecker->canDeleteMarketOpenOrder(instrument));
    s = s + "\ncanCreateMarketCloseOrder: " + getPermissionStatusText(permissionChecker->canCreateMarketCloseOrder(instrument));
    s = s + "\ncanChangeMarketCloseOrder: " + getPermissionStatusText(permissionChecker->canChangeMarketCloseOrder(instrument));
    s = s + "\ncanDeleteMarketCloseOrder: " + getPermissionStatusText(permissionChecker->canDeleteMarketCloseOrder(instrument));
    s = s + "\ncanCreateEntryOrder: " + getPermissionStatusText(permissionChecker->canCreateEntryOrder(instrument));
    s = s + "\ncanChangeEntryOrder: " + getPermissionStatusText(permissionChecker->canChangeEntryOrder(instrument));
    s = s + "\ncanDeleteEntryOrder: " + getPermissionStatusText(permissionChecker->canDeleteEntryOrder(instrument));
    s = s + "\ncanCreateStopLimitOrder: " + getPermissionStatusText(permissionChecker->canCreateStopLimitOrder(instrument));
    s = s + "\ncanChangeStopLimitOrder: " + getPermissionStatusText(permissionChecker->canChangeStopLimitOrder(instrument));
    s = s + "\ncanDeleteStopLimitOrder: " + getPermissionStatusText(permissionChecker->canDeleteStopLimitOrder(instrument));
    s = s + "\ncanRequestQuote: " + getPermissionStatusText(permissionChecker->canRequestQuote(instrument));
    s = s + "\ncanAcceptQuote: " + getPermissionStatusText(permissionChecker->canAcceptQuote(instrument));
    s = s + "\ncanDeleteQuote: " + getPermissionStatusText(permissionChecker->canDeleteQuote(instrument));
    s = s + "\ncanCreateOCO: " + getPermissionStatusText(permissionChecker->canCreateOCO(instrument));
    s = s + "\ncanCreateOTO: " + getPermissionStatusText(permissionChecker->canCreateOTO(instrument));
    s = s + "\ncanJoinToNewContingencyGroup: " + getPermissionStatusText(permissionChecker->canJoinToNewContingencyGroup(instrument));
    s = s + "\ncanJoinToExistingContingencyGroup: " + getPermissionStatusText(permissionChecker->canJoinToExistingContingencyGroup(instrument));
    s = s + "\ncanRemoveFromContingencyGroup: " + getPermissionStatusText(permissionChecker->canRemoveFromContingencyGroup(instrument));
    s = s + "\ncanChangeOfferSubscription: " + getPermissionStatusText(permissionChecker->canChangeOfferSubscription(instrument));
    s = s + "\ncanCreateNetCloseOrder: " + getPermissionStatusText(permissionChecker->canCreateNetCloseOrder(instrument));
    s = s + "\ncanChangeNetCloseOrder: " + getPermissionStatusText(permissionChecker->canChangeNetCloseOrder(instrument));
    s = s + "\ncanDeleteNetCloseOrder: " + getPermissionStatusText(permissionChecker->canDeleteNetCloseOrder(instrument));
    s = s + "\ncanCreateNetStopLimitOrder: " + getPermissionStatusText(permissionChecker->canCreateNetStopLimitOrder(instrument));
    s = s + "\ncanChangeNetStopLimitOrder: " + getPermissionStatusText(permissionChecker->canChangeNetStopLimitOrder(instrument));
    s = s + "\ncanDeleteNetStopLimitOrder: " + getPermissionStatusText(permissionChecker->canDeleteNetStopLimitOrder(instrument));
    s = s + "\ncanUseDynamicTrailingForStop: " + getPermissionStatusText(permissionChecker->canUseDynamicTrailingForStop());
    s = s + "\ncanUseDynamicTrailingForLimit: " + getPermissionStatusText(permissionChecker->canUseDynamicTrailingForLimit());
    s = s + "\ncanUseDynamicTrailingForEntryStop: " + getPermissionStatusText(permissionChecker->canUseDynamicTrailingForEntryStop());
    s = s + "\ncanUseDynamicTrailingForEntryLimit: " + getPermissionStatusText(permissionChecker->canUseDynamicTrailingForEntryLimit());
    s = s + "\ncanUseFluctuateTrailingForStop: " + getPermissionStatusText(permissionChecker->canUseFluctuateTrailingForStop());
    s = s + "\ncanUseFluctuateTrailingForLimit: " + getPermissionStatusText(permissionChecker->canUseFluctuateTrailingForLimit());
    s = s + "\ncanUseFluctuateTrailingForEntryStop: " + getPermissionStatusText(permissionChecker->canUseFluctuateTrailingForEntryStop());
    s = s + "\ncanUseFluctuateTrailingForEntryLimit: " + getPermissionStatusText(permissionChecker->canUseFluctuateTrailingForEntryLimit());
  }
  m_EventQueue.push(std::move(s));
}

void Order2Go::logSettings(IO2GLoginRules *loginRules)
{
  std::string s = "[Settings]";
  O2G2Ptr<IO2GResponse> accountsResponse = loginRules->getTableRefreshResponse(::Accounts);
  if (!accountsResponse) {
    return;
  }
  O2G2Ptr<IO2GResponse> offersResponse = loginRules->getTableRefreshResponse(::Offers);
  if (!offersResponse) {
    return;
  }

  O2G2Ptr<IO2GTradingSettingsProvider> tradingSettingsProvider = loginRules->getTradingSettingsProvider();
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (!factory) {
    return;
  }
  O2G2Ptr<IO2GAccountsTableResponseReader> accountsReader = factory->createAccountsTableReader(accountsResponse);
  if (!accountsReader) {
    return;
  }
  O2G2Ptr<IO2GOffersTableResponseReader> instrumentsReader = factory->createOffersTableReader(offersResponse);
  if (!instrumentsReader) {
    return;
  }
  O2G2Ptr<IO2GAccountRow> account = accountsReader->getRow(0);
  if (!account) {
    return;
  }

  for (int i = 0; i < instrumentsReader->size(); ++i) {
    O2G2Ptr<IO2GOfferRow> instrumentRow = instrumentsReader->getRow(i);
    const char* instrument = instrumentRow->getInstrument();
    int condDistStopForTrade = tradingSettingsProvider->getCondDistStopForTrade(instrument);
    int condDistLimitForTrade = tradingSettingsProvider->getCondDistLimitForTrade(instrument);
    int condDistEntryStop = tradingSettingsProvider->getCondDistEntryStop(instrument);
    int condDistEntryLimit = tradingSettingsProvider->getCondDistEntryLimit(instrument);
    int minQuantity = tradingSettingsProvider->getMinQuantity(instrument, account);
    int maxQuantity = tradingSettingsProvider->getMaxQuantity(instrument, account);
    int baseUnitSize = tradingSettingsProvider->getBaseUnitSize(instrument, account);
    O2GMarketStatus marketStatus = tradingSettingsProvider->getMarketStatus(instrument);
    int minTrailingStep = tradingSettingsProvider->getMinTrailingStep();
    int maxTrailingStep = tradingSettingsProvider->getMaxTrailingStep();
    double mmr = tradingSettingsProvider->getMMR(instrument, account);
    double mmr2, emr, lmr;
    bool threeLevelMargin = tradingSettingsProvider->getMargins(instrument, account, mmr2, emr, lmr);

    char buffer[256];
    const char* marketStatusStr = "Unknown";
    switch (marketStatus) {
    case ::MarketStatusOpen:
      marketStatusStr = "Market Open";
      break;
    case ::MarketStatusClosed:
      marketStatusStr = "Market Close";
      break;
    case ::MarketStatusUndefined:
      marketStatusStr = "Market Undefined";
      break;
    }

    sprintf_s(buffer, sizeof(buffer), "\n  [%s (%s)] Cond.Dist: ST=%d; LT=%d", instrument, marketStatusStr, condDistStopForTrade, condDistLimitForTrade);
    s += buffer;
    sprintf_s(buffer, sizeof(buffer), "\n            Cond.Dist entry stop=%d; entry limit=%d", condDistEntryStop, condDistEntryLimit);
    s += buffer;
    sprintf_s(buffer, sizeof(buffer), "\n            Quantity: Min=%d; Max=%d. Base unit size=%d; MMR=%f", minQuantity, maxQuantity, baseUnitSize, mmr);
    s += buffer;
    if (threeLevelMargin) {
      sprintf_s(buffer, sizeof(buffer), "\n            Three level margin: MMR=%f; EMR=%f; LMR=%f", mmr2, emr, lmr);
    } else {
      sprintf_s(buffer, sizeof(buffer), "\n            Single level margin: MMR=%f; EMR=%f; LMR=%f", mmr2, emr, lmr);
    }
    s += buffer;
    sprintf_s(buffer, sizeof(buffer), "\n            Trailing step: %d-%d", minTrailingStep, maxTrailingStep);
    s += buffer;
  }
  m_EventQueue.push(std::move(s));
}

std::string Order2Go::getPermissionStatusText(O2GPermissionStatus permissionStatus)
{
  std::string statusStr = "Unknown";
  switch (permissionStatus) {
  case ::PermissionDisabled:
    statusStr = "Disabled";
    break;
  case ::PermissionEnabled:
    statusStr = "Enabled";
    break;
  case ::PermissionHidden:
    statusStr = "Hidden";
    break;
  default:
    break;
  }
  return statusStr;
}

LoadDataWorker::LoadDataWorker(const Napi::Function &callback, Order2Go *order2Go,
                               SymbolsType &symbols, PeriodsType &periods, int priceHistoryCount)
  : Napi::AsyncWorker(callback), m_Order2Go(order2Go), m_Symbols(std::move(symbols)),
    m_Periods(std::move(periods)), m_PriceHistoryCount(priceHistoryCount) {};

void LoadDataWorker::Execute()
{
  m_Order2Go->loadData(m_Symbols, m_Periods, m_PriceHistoryCount);
};

void LoadDataWorker::OnOK()
{
  Callback().Call({m_Order2Go->valueOf(Env())});
};

GetHistoricalPricesWorker::GetHistoricalPricesWorker(const Napi::Function &callback, Order2Go *order2Go,
                                                     std::string &symbol, std::string &period,
                                                     time_t startTime, time_t endTime)
  : Napi::AsyncWorker(callback), m_Order2Go(order2Go), m_Symbol(std::move(symbol)),
    m_Period(std::move(period)), m_StartTime(startTime), m_EndTime(endTime) {};

void GetHistoricalPricesWorker::Execute()
{
  m_Order2Go->getHistoricalPrices(m_Symbol, m_Period, m_StartTime, m_EndTime, m_Candles, m_Error);
};

void GetHistoricalPricesWorker::OnOK()
{
  Napi::Env env = Env();
  Napi::Object obj = Napi::Object::New(env);

  Napi::Array candleArray = Napi::Array::New(env, m_Candles.size());
  uint32_t i = 0;
  for (const auto &candle : m_Candles) {
    candleArray[i] = candle->toValue(env);
    i++;
  }

  obj.Set(Napi::String::New(env, "data"), candleArray);
  obj.Set(Napi::String::New(env, "error"), Napi::String::New(env, m_Error));
  Callback().Call({obj});
};
