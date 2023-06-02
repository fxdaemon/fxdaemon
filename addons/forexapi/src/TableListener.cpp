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
#include "TableListener.h"

static const char *ContingencyType[] = { "No", "OCO", "OTO", "ELS", "OTOCO" };
static const char *RateType[] = { "None", "NonPegged", "PeggedOpen", "PeggedClose" };
static const char *InstrumentType[] = {"Forex", "Indices", "Commodity", "Treasury", 
                                       "Bullion", "Shares", "FXIndex", "CFDShares", "Crypto"};

static const char *getContingencyTypeText(O2GContingencyType contingencyType)
{
  int index = static_cast<int>(contingencyType);
  if (index >= 0 && index < (int)(sizeof(ContingencyType) / sizeof(char*))) {
    return ContingencyType[index];
  }
  return ContingencyType[0];
}

static const char *getRateTypeText(O2GRateType rateType)
{
  int index = static_cast<int>(rateType);
  if (index >= 0 && index < (int)(sizeof(RateType) / sizeof(char*))) {
    return RateType[index];
  }
  return RateType[0];
}

static const char *getInstrumentTypeText(O2GInstrumentType instrumentType)
{
  int index = static_cast<int>(instrumentType) - 1;
  if (index >= 0 && index < (int)(sizeof(InstrumentType) / sizeof(char*))) {
    return InstrumentType[index];
  }
  return InstrumentType[0];
}

TableListener::TableListener(EventQueue *eventQueue)
  : m_EventQueue(eventQueue)
{
  m_Subscribed = false;
}

void TableListener::onStatusChanged(O2GTableStatus status)
{
}

void TableListener::onAdded(const char *rowID, IO2GRow *row)
{
  onTableRow(row, fxdaemon::Table::ADDED);
}

void TableListener::onChanged(const char *rowID, IO2GRow *row)
{
  onTableRow(row, fxdaemon::Table::CHANGED);
}

void TableListener::onDeleted(const char *rowID, IO2GRow *row)
{
  onTableRow(row, fxdaemon::Table::DELETED);
}

void TableListener::subscribeEvents(IO2GTableManager *manager)
{
  O2G2Ptr<IO2GAccountsTable> accountsTable = (IO2GAccountsTable*)manager->getTable(::Accounts);
  O2G2Ptr<IO2GOrdersTable> ordersTable = (IO2GOrdersTable *)manager->getTable(::Orders);
  O2G2Ptr<IO2GTradesTable> tradesTable = (IO2GTradesTable*)manager->getTable(::Trades);
  O2G2Ptr<IO2GClosedTradesTable> closeTradesTable = (IO2GClosedTradesTable*)manager->getTable(::ClosedTrades);
  O2G2Ptr<IO2GSummaryTable> summaryTable = (IO2GSummaryTable*)manager->getTable(::Summary);
  O2G2Ptr<IO2GMessagesTable> messageTable = (IO2GMessagesTable*)manager->getTable(::Messages);
  accountsTable->subscribeUpdate(::Update, this);
  ordersTable->subscribeUpdate(::Insert, this);
  ordersTable->subscribeUpdate(::Update, this);
  ordersTable->subscribeUpdate(::Delete, this);
  tradesTable->subscribeUpdate(::Insert, this);
  tradesTable->subscribeUpdate(::Update, this);
  tradesTable->subscribeUpdate(::Delete, this);
  closeTradesTable->subscribeUpdate(::Insert, this);
  summaryTable->subscribeUpdate(::Insert, this);
  summaryTable->subscribeUpdate(::Update, this);
  summaryTable->subscribeUpdate(::Delete, this);
  messageTable->subscribeUpdate(::Insert, this);
  m_Subscribed = true;
}

void TableListener::unsubscribeEvents(IO2GTableManager *manager)
{
  if (m_Subscribed) {
    O2G2Ptr<IO2GAccountsTable> accountsTable = (IO2GAccountsTable*)manager->getTable(::Accounts);
    O2G2Ptr<IO2GOrdersTable> ordersTable = (IO2GOrdersTable *)manager->getTable(::Orders);
    O2G2Ptr<IO2GTradesTable> tradesTable = (IO2GTradesTable*)manager->getTable(::Trades);
    O2G2Ptr<IO2GClosedTradesTable> closeTradesTable = (IO2GClosedTradesTable*)manager->getTable(::ClosedTrades);
    O2G2Ptr<IO2GSummaryTable> summaryTable = (IO2GSummaryTable*)manager->getTable(::Summary);
    O2G2Ptr<IO2GMessagesTable> messageTable = (IO2GMessagesTable*)manager->getTable(::Messages);
    accountsTable->unsubscribeUpdate(::Update, this);
    ordersTable->unsubscribeUpdate(::Insert, this);
    ordersTable->unsubscribeUpdate(::Update, this);
    ordersTable->unsubscribeUpdate(::Delete, this);
    tradesTable->unsubscribeUpdate(::Insert, this);
    tradesTable->unsubscribeUpdate(::Update, this);
    tradesTable->unsubscribeUpdate(::Delete, this);
    closeTradesTable->unsubscribeUpdate(::Insert, this);
    summaryTable->unsubscribeUpdate(::Insert, this);
    summaryTable->unsubscribeUpdate(::Update, this);
    summaryTable->unsubscribeUpdate(::Delete, this);
    messageTable->unsubscribeUpdate(::Insert, this);
  }
  m_Subscribed = false;
}

void TableListener::onTableRow(IO2GRow *row, fxdaemon::Table::Status status)
{
  Event event = {Event::UNKNOWN, NULL};
  switch (row->getTableType()) {
  case ::Offers:
    event.type = Event::OFFER;
    event.data = valueOfRow(static_cast<IO2GOfferTableRow*>(row), status);
    break;
  case ::Accounts:
    event.type = Event::ACCOUNT;
    event.data = valueOfRow(static_cast<IO2GAccountTableRow*>(row), status);
    break;
  case ::Orders:
    event.type = Event::ORDER;
    event.data = valueOfRow(static_cast<IO2GOrderTableRow*>(row), status);
    break;
  case ::Trades:
    event.type = Event::TRADE;
    event.data = valueOfRow(static_cast<IO2GTradeTableRow*>(row), status);
    break;
  case ::ClosedTrades:
    event.type = Event::CLOSED_TRADE;
    event.data = valueOfRow(static_cast<IO2GClosedTradeTableRow*>(row), status);
    break;
  case ::Messages:
    event.type = Event::MESSAGE;
    event.data = valueOfRow(static_cast<IO2GMessageRow*>(row), status);
    break;
  case ::Summary:
    event.type = Event::SUMMARY;
    event.data = valueOfRow(static_cast<IO2GSummaryTableRow*>(row), status);
    break;
  default:
    break;
  }

  if (event.data) {
    m_EventQueue->push(event);
  }
}

std::shared_ptr<fxdaemon::Account> TableListener::valueOfRow(
  IO2GAccountRow *accountRow, fxdaemon::Table::Status status)
{
  std::shared_ptr<fxdaemon::Account> account = std::make_shared<fxdaemon::Account>(status);
  account->setAccountID(accountRow->getAccountID());
  account->setAccountName(accountRow->getAccountName());
  account->setAccountKind(accountRow->getAccountKind());
  account->setBalance(accountRow->getBalance());
  account->setNonTradeEquity(accountRow->getNonTradeEquity());
  account->setM2MEquity(accountRow->getM2MEquity());
  account->setMarginCallFlag(accountRow->getMarginCallFlag());
  account->setLastMarginCallDate(Utils::oleTime2Time(accountRow->getLastMarginCallDate()));
  account->setMaintenanceType(accountRow->getMaintenanceType());
  account->setAmountLimit(accountRow->getAmountLimit());
  account->setBaseUnitSize(accountRow->getBaseUnitSize());
  account->setMaintenanceFlag(accountRow->getMaintenanceFlag());
  account->setManagerAccountID(accountRow->getManagerAccountID());
  account->setLeverageProfileID(accountRow->getLeverageProfileID());
  account->setATPID(accountRow->getATPID());
  return account;
}

std::shared_ptr<fxdaemon::Account> TableListener::valueOfRow(
  IO2GAccountTableRow *accountRow, fxdaemon::Table::Status status)
{
  std::shared_ptr<fxdaemon::Account> account = valueOfRow(static_cast<IO2GAccountRow*>(accountRow), status);
  account->setUsedMargin(accountRow->getUsedMargin());
  account->setEquity(accountRow->getEquity());
  account->setDayPL(accountRow->getDayPL());
  account->setUsableMargin(accountRow->getUsableMargin());
  account->setGrossPL(accountRow->getGrossPL());
  account->setUsableMarginInPercentage(accountRow->getUsableMarginInPercentage());
  account->setUsableMaintMarginInPercentage(accountRow->getUsableMaintMarginInPercentage());
  return account;
}

std::shared_ptr<fxdaemon::Offer> TableListener::valueOfRow(
  IO2GOfferRow *offerRow, fxdaemon::Table::Status status)
{
  std::shared_ptr<fxdaemon::Offer> offer = std::make_shared<fxdaemon::Offer>(status);
  offer->setOfferID(offerRow->getOfferID());
  offer->setSymbol(offerRow->getInstrument());
  offer->setBid(offerRow->getBid());
  offer->setAsk(offerRow->getAsk());
  offer->setHigh(offerRow->getHigh());
  offer->setLow(offerRow->getLow());
  offer->setVolume(offerRow->getVolume());
  offer->setContractCurrency(offerRow->getContractCurrency());
  offer->setTime(Utils::oleTime2Time(offerRow->getTime()));
  offer->setSellInterest(offerRow->getSellInterest());
  offer->setBuyInterest(offerRow->getBuyInterest());
  offer->setDigits(offerRow->getDigits());
  offer->setInstrumentType(getInstrumentTypeText(offerRow->getInstrumentType()));
  offer->setContractMultiplier(offerRow->getContractMultiplier());
  offer->setValueDate(offerRow->getValueDate());
  if (offerRow->getPointSize() > 0) {
    offer->setPointSize(offerRow->getPointSize());
  }
  return offer;
}

std::shared_ptr<fxdaemon::Offer> TableListener::valueOfRow(
  IO2GOfferTableRow *offerRow, fxdaemon::Table::Status status)
{
  std::shared_ptr<fxdaemon::Offer> offer = valueOfRow(static_cast<IO2GOfferRow*>(offerRow), status);
  offer->setPipCost(offerRow->getPipCost());
  return offer;
}

std::shared_ptr<fxdaemon::Order> TableListener::valueOfRow(
  IO2GOrderRow *orderRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::Order> order = std::make_shared<fxdaemon::Order>(status);
	order->setOrderID(orderRow->getOrderID());
	order->setRequestID(orderRow->getRequestID());
	order->setAccountID(orderRow->getAccountID());
	order->setOfferID(orderRow->getOfferID());
	order->setTradeID(orderRow->getTradeID());
	order->setNetQuantity(orderRow->getNetQuantity());
	order->setBS(orderRow->getBuySell());
	order->setStage(orderRow->getStage());
	order->setType(orderRow->getType());
	order->setOrderStatus(orderRow->getStatus());
	order->setAmount(orderRow->getAmount());
	order->setRate(orderRow->getRate());
  order->setExecutionRate(orderRow->getExecutionRate());
  order->setRateMin(orderRow->getRateMin());
	order->setRateMax(orderRow->getRateMax());
	order->setStatusTime(Utils::oleTime2Time(orderRow->getStatusTime()));
	order->setAtMarket(orderRow->getAtMarket());
	order->setTrailStep(orderRow->getTrailStep());
	order->setTrailRate(orderRow->getTrailRate());
	order->setTimeInForce(orderRow->getTimeInForce());
	order->setContingentOrderID(orderRow->getContingentOrderID());
	order->setContingencyType(getContingencyTypeText(orderRow->getContingencyType()));
  order->setPrimaryID(orderRow->getPrimaryID());
  order->setOriginAmount(orderRow->getOriginAmount());
  order->setFilledAmount(orderRow->getFilledAmount());
  order->setWorkingIndicator(orderRow->getWorkingIndicator());
  order->setPegType(orderRow->getPegType());
  order->setPegOffset(orderRow->getPegOffset());
  order->setExpireTime(Utils::oleTime2Time(orderRow->getExpireDate()));
  order->setValueDate(orderRow->getValueDate());
	return order;
}

std::shared_ptr<fxdaemon::Order> TableListener::valueOfRow(
  IO2GOrderTableRow *orderRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::Order> order = valueOfRow((IO2GOrderRow*)orderRow, status);
	order->setStop(orderRow->getStop());
	order->setLimit(orderRow->getLimit());
  order->setTypeStop(getRateTypeText(orderRow->getTypeStop()));
  order->setTypeLimit(getRateTypeText(orderRow->getTypeLimit()));
	return order;
}

std::shared_ptr<fxdaemon::Trade> TableListener::valueOfRow(
  IO2GTradeRow *tradeRow, fxdaemon::Table::Status status)
{
  std::shared_ptr<fxdaemon::Trade> trade = std::make_shared<fxdaemon::Trade>(status);
	trade->setTradeID(tradeRow->getTradeID());
	trade->setAccountID(tradeRow->getAccountID());
	trade->setOfferID(tradeRow->getOfferID());
	trade->setAmount(tradeRow->getAmount());
	trade->setBS(tradeRow->getBuySell());
	trade->setOpenRate(tradeRow->getOpenRate());
  trade->setOpenTime(Utils::oleTime2Time(tradeRow->getOpenTime()));
	trade->setOpenQuoteID(tradeRow->getOpenQuoteID());
	trade->setOpenOrderID(tradeRow->getOpenOrderID());
	trade->setOpenOrderReqID(tradeRow->getOpenOrderReqID());
	trade->setOpenOrderRequestTXT(tradeRow->getOpenOrderRequestTXT());
  trade->setCommission(tradeRow->getCommission());
	trade->setRolloverInterest(tradeRow->getRolloverInterest());
  trade->setTradeIDOrigin(tradeRow->getTradeIDOrigin());
  trade->setValueDate(tradeRow->getValueDate());
  trade->setDividends(tradeRow->getDividends());
	return trade;
}

std::shared_ptr<fxdaemon::Trade> TableListener::valueOfRow(
  IO2GTradeTableRow *tradeRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::Trade> trade = valueOfRow((IO2GTradeRow*)tradeRow, status);
  trade->setUsedMargin(tradeRow->getUsedMargin());
  trade->setPL(tradeRow->getPL());
	trade->setGrossPL(tradeRow->getGrossPL());
	trade->setClose(tradeRow->getClose());
	trade->setStop(tradeRow->getStop());
	trade->setLimit(tradeRow->getLimit());	
	return trade;
}

std::shared_ptr<fxdaemon::ClosedTrade> TableListener::valueOfRow(
  IO2GClosedTradeRow *tradeRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::ClosedTrade> closedTrade = std::make_shared<fxdaemon::ClosedTrade>(status);
	closedTrade->setTradeID(tradeRow->getTradeID());
	closedTrade->setAccountID(tradeRow->getAccountID());
	closedTrade->setOfferID(tradeRow->getOfferID());
	closedTrade->setAmount(tradeRow->getAmount());
	closedTrade->setBS(tradeRow->getBuySell());
	closedTrade->setOpenRate(tradeRow->getOpenRate());
  closedTrade->setOpenTime(Utils::oleTime2Time(tradeRow->getOpenTime()));
  closedTrade->setGrossPL(tradeRow->getGrossPL());
  closedTrade->setCommission(tradeRow->getCommission());
	closedTrade->setRolloverInterest(tradeRow->getRolloverInterest());
  closedTrade->setOpenQuoteID(tradeRow->getOpenQuoteID());
	closedTrade->setOpenOrderID(tradeRow->getOpenOrderID());
	closedTrade->setOpenOrderReqID(tradeRow->getOpenOrderReqID());
	closedTrade->setOpenOrderRequestTXT(tradeRow->getOpenOrderRequestTXT());
	closedTrade->setCloseRate(tradeRow->getCloseRate());
  closedTrade->setCloseTime(Utils::oleTime2Time(tradeRow->getCloseTime()));
	closedTrade->setCloseQuoteID(tradeRow->getCloseQuoteID());
	closedTrade->setCloseOrderID(tradeRow->getCloseOrderID());
	closedTrade->setCloseOrderReqID(tradeRow->getCloseOrderReqID());
	closedTrade->setCloseOrderRequestTXT(tradeRow->getCloseOrderRequestTXT());
  closedTrade->setTradeIDOrigin(tradeRow->getTradeIDOrigin());
  closedTrade->setTradeIDRemain(tradeRow->getTradeIDRemain());
  closedTrade->setValueDate(tradeRow->getValueDate());
  closedTrade->setDividends(tradeRow->getDividends());
	return closedTrade;
}

std::shared_ptr<fxdaemon::ClosedTrade> TableListener::valueOfRow(
  IO2GClosedTradeTableRow *tradeRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::ClosedTrade> closedTrade = valueOfRow((IO2GClosedTradeRow*)tradeRow, status);
	closedTrade->setPL(tradeRow->getPL());
  closedTrade->setNetPL(tradeRow->getNetPL());
	return closedTrade;
}

std::shared_ptr<fxdaemon::Summary> TableListener::valueOfRow(
  IO2GSummaryTableRow *summaryRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::Summary> summary = std::make_shared<fxdaemon::Summary>(status);
	summary->setOfferID(summaryRow->getOfferID());
	summary->setSellNetPL(summaryRow->getSellNetPL());
  summary->setSellNetPLPip(summaryRow->getSellNetPLPip());
	summary->setSellAmount(summaryRow->getSellAmount());
	summary->setSellAvgOpen(summaryRow->getSellAvgOpen());
	summary->setBuyClose(summaryRow->getBuyClose());
	summary->setSellClose(summaryRow->getSellClose());
	summary->setBuyAvgOpen(summaryRow->getBuyAvgOpen());
	summary->setBuyAmount(summaryRow->getBuyAmount());
	summary->setBuyNetPL(summaryRow->getBuyNetPL());
  summary->setBuyNetPLPip(summaryRow->getBuyNetPLPip());  
	summary->setAmount(summaryRow->getAmount());
	summary->setGrossPL(summaryRow->getGrossPL());
  summary->setNetPL(summaryRow->getNetPL());
  summary->setRolloverInterestSum(summaryRow->getRolloverInterestSum());
  summary->setUsedMargin(summaryRow->getUsedMargin());
  summary->setUsedMarginBuy(summaryRow->getUsedMarginBuy());
  summary->setUsedMarginSell(summaryRow->getUsedMarginSell());
  summary->setCommission(summaryRow->getCommission());
  summary->setCloseCommission(summaryRow->getCloseCommission());
  summary->setDividends(summaryRow->getDividends());
	return summary;
}

std::shared_ptr<fxdaemon::Message> TableListener::valueOfRow(
  IO2GMessageRow *messageRow, fxdaemon::Table::Status status)
{
	std::shared_ptr<fxdaemon::Message> message = std::make_shared<fxdaemon::Message>(status);
	message->setMsgID(messageRow->getMsgID());
  message->setTime(Utils::oleTime2Time(messageRow->getTime()));
	message->setFrom(messageRow->getFrom());
  message->setType(messageRow->getType());
  message->setFeature(messageRow->getFeature());
	message->setText(messageRow->getText());
	message->setSubject(messageRow->getSubject());
	message->setHTMLFragmentFlag(messageRow->getHTMLFragmentFlag());
	return message;
}
