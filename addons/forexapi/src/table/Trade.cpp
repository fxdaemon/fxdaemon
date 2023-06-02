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
#include "../stdafx.h"
#include "Trade.h"

using namespace fxdaemon;

Trade::Trade(Status status) : Table(status)
{
  m_Amount = 0;
  m_OpenRate = 0;
  m_OpenTime = 0;
  m_UsedMargin = 0;
  m_PL = 0;
  m_GrossPL = 0;
  m_Close = 0;
  m_Stop = 0;
  m_Limit = 0;
  m_Commission = 0;
  m_RolloverInterest = 0;
  m_Dividends = 0;
}

const std::string &Trade::getTradeID() const
{
  return m_TradeID;
}

void Trade::setTradeID(const char *tradeID)
{
  m_TradeID = tradeID;
}

const std::string &Trade::getAccountID() const
{
  return m_AccountID;
}

void Trade::setAccountID(const char *accountID)
{
  m_AccountID = accountID;
}

const std::string &Trade::getOfferID() const
{
  return m_OfferID;
}

void Trade::setOfferID(const char *offerID)
{
  m_OfferID = offerID;
}

double Trade::getAmount() const
{
  return m_Amount;
}

void Trade::setAmount(double amount)
{
  m_Amount = amount;
}

const std::string &Trade::getBS() const
{
  return m_BS;
}

void Trade::setBS(const char *bs)
{
  m_BS = bs;
}

double Trade::getOpenRate() const
{
  return m_OpenRate;
}

void Trade::setOpenRate(double openRate)
{
  m_OpenRate = openRate;
}

time_t Trade::getOpenTime() const
{
  return m_OpenTime;
}

void Trade::setOpenTime(time_t openTime)
{
  m_OpenTime = openTime;
}

double Trade::getUsedMargin() const
{
  return m_UsedMargin;
}

void Trade::setUsedMargin(double usedMargin)
{
  m_UsedMargin = usedMargin;
}

double Trade::getPL() const
{
  return m_PL;
}

void Trade::setPL(double pl)
{
  m_PL = pl;
}

double Trade::getGrossPL() const
{
  return m_GrossPL;
}

void Trade::setGrossPL(double grossPL)
{
  m_GrossPL = grossPL;
}

double Trade::getClose() const
{
  return m_Close;
}

void Trade::setClose(double close)
{
  m_Close = close;
}

double Trade::getStop() const
{
  return m_Stop;
}

void Trade::setStop(double stop)
{
  m_Stop = stop;
}

double Trade::getLimit() const
{
  return m_Limit;
}

void Trade::setLimit(double limit)
{
  m_Limit = limit;
}

const std::string &Trade::getOpenQuoteID() const
{
  return m_OpenQuoteID;
}

void Trade::setOpenQuoteID(const char *openQuoteID)
{
  m_OpenQuoteID = openQuoteID;
}

const std::string &Trade::getOpenOrderID() const
{
  return m_OpenOrderID;
}

void Trade::setOpenOrderID(const char *openOrderID)
{
  m_OpenOrderID = openOrderID;
}

const std::string &Trade::getOpenOrderReqID() const
{
  return m_OpenOrderReqID;
}

void Trade::setOpenOrderReqID(const char *openOrderReqID)
{
  m_OpenOrderReqID = openOrderReqID;
}

const std::string &Trade::getOpenOrderRequestTXT() const
{
  return m_OpenOrderRequestTXT;
}

void Trade::setOpenOrderRequestTXT(const char *openOrderRequestTXT)
{
  m_OpenOrderRequestTXT = openOrderRequestTXT;
}

double Trade::getCommission() const
{
  return m_Commission;
}

void Trade::setCommission(double commission)
{
  m_Commission = commission;
}

double Trade::getRolloverInterest() const
{
  return m_RolloverInterest;
}

void Trade::setRolloverInterest(double rolloverInterest)
{
  m_RolloverInterest = rolloverInterest;
}

const std::string &Trade::getTradeIDOrigin() const
{
  return m_TradeIDOrigin;
}

void Trade::setTradeIDOrigin(const char *tradeIDOrigin)
{
  m_TradeIDOrigin = tradeIDOrigin;
}

const std::string &Trade::getValueDate() const
{
  return m_ValueDate;
}

void Trade::setValueDate(const char *valueDate)
{
  m_ValueDate = valueDate;
}

double Trade::getDividends() const
{
  return m_Dividends;
}

void Trade::setDividends(double dividends)
{
  m_Dividends = dividends;
}

Napi::Value Trade::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "trade_id"), Napi::String::New(env, m_TradeID));
  obj.Set(Napi::String::New(env, "account_id"), Napi::String::New(env, m_AccountID));
  obj.Set(Napi::String::New(env, "offer_id"), Napi::String::New(env, m_OfferID));
  obj.Set(Napi::String::New(env, "amount"), Napi::Number::New(env, m_Amount));
  obj.Set(Napi::String::New(env, "side"), Napi::String::New(env, m_BS));
  obj.Set(Napi::String::New(env, "open_rate"), Napi::Number::New(env, m_OpenRate));
  obj.Set(Napi::String::New(env, "open_time"), Napi::Number::New(env, m_OpenTime));
  obj.Set(Napi::String::New(env, "used_margin"), Napi::Number::New(env, m_UsedMargin));
  obj.Set(Napi::String::New(env, "pl"), Napi::Number::New(env, m_PL));
  obj.Set(Napi::String::New(env, "gross_pl"), Napi::Number::New(env, m_GrossPL));
  obj.Set(Napi::String::New(env, "close"), Napi::Number::New(env, m_Close));
  obj.Set(Napi::String::New(env, "stop"), Napi::Number::New(env, m_Stop));
  obj.Set(Napi::String::New(env, "limit"), Napi::Number::New(env, m_Limit));
  obj.Set(Napi::String::New(env, "open_quote_id"), Napi::String::New(env, m_OpenQuoteID));
  obj.Set(Napi::String::New(env, "open_order_id"), Napi::String::New(env, m_OpenOrderID));  
  obj.Set(Napi::String::New(env, "open_order_req_id"), Napi::String::New(env, m_OpenOrderReqID));
  obj.Set(Napi::String::New(env, "open_order_request_txt"), Napi::String::New(env, m_OpenOrderRequestTXT));
  obj.Set(Napi::String::New(env, "commission"), Napi::Number::New(env, m_Commission));
  obj.Set(Napi::String::New(env, "rollover_interest"), Napi::Number::New(env, m_RolloverInterest));
  obj.Set(Napi::String::New(env, "trade_id_origin"), Napi::String::New(env, m_TradeIDOrigin));
  obj.Set(Napi::String::New(env, "value_date"), Napi::String::New(env, m_ValueDate));
  obj.Set(Napi::String::New(env, "dividends"), Napi::Number::New(env, m_Dividends));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}

ClosedTrade::ClosedTrade(Status status) : Trade(status)
{
  m_Amount = 0;
  m_OpenRate = 0;
  m_OpenTime = 0;
  m_CloseRate = 0;
  m_CloseTime = 0;
  m_PL = 0;
  m_NetPL = 0;
  m_GrossPL = 0;
  m_Commission = 0;
  m_RolloverInterest = 0;
  m_Dividends = 0;
}

double ClosedTrade::getCloseRate() const
{
  return m_CloseRate;
}

void ClosedTrade::setCloseRate(double closeRate)
{
  m_CloseRate = closeRate;
}

time_t ClosedTrade::getCloseTime() const
{
  return m_CloseTime;
}

void ClosedTrade::setCloseTime(time_t closeTime)
{
  m_CloseTime = closeTime;
}

double ClosedTrade::getNetPL() const
{
  return m_NetPL;
}

void ClosedTrade::setNetPL(double netPL)
{
  m_NetPL = netPL;
}

const std::string &ClosedTrade::getCloseQuoteID() const
{
  return m_CloseQuoteID;
}

void ClosedTrade::setCloseQuoteID(const char *closeQuoteID)
{
  m_CloseQuoteID = closeQuoteID;
}

const std::string &ClosedTrade::getCloseOrderID() const
{
  return m_CloseOrderID;
}

void ClosedTrade::setCloseOrderID(const char *closeOrderID)
{
  m_CloseOrderID = closeOrderID;
}

const std::string &ClosedTrade::getCloseOrderReqID() const
{
  return m_CloseOrderReqID;
}

void ClosedTrade::setCloseOrderReqID(const char *closeOrderReqID)
{
  m_CloseOrderReqID = closeOrderReqID;
}

const std::string &ClosedTrade::getCloseOrderRequestTXT() const
{
  return m_CloseOrderRequestTXT;
}

void ClosedTrade::setCloseOrderRequestTXT(const char *closeOrderRequestTXT)
{
  m_CloseOrderRequestTXT = closeOrderRequestTXT;
}

const std::string &ClosedTrade::getTradeIDRemain() const
{
  return m_TradeIDRemain;
}

void ClosedTrade::setTradeIDRemain(const char *tradeIDRemain)
{
  m_TradeIDRemain = tradeIDRemain;
}

Napi::Value ClosedTrade::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "trade_id"), Napi::String::New(env, m_TradeID));
  obj.Set(Napi::String::New(env, "account_id"), Napi::String::New(env, m_AccountID));
  obj.Set(Napi::String::New(env, "offer_id"), Napi::String::New(env, m_OfferID));
  obj.Set(Napi::String::New(env, "amount"), Napi::Number::New(env, m_Amount));
  obj.Set(Napi::String::New(env, "side"), Napi::String::New(env, m_BS));
  obj.Set(Napi::String::New(env, "open_rate"), Napi::Number::New(env, m_OpenRate));
  obj.Set(Napi::String::New(env, "open_time"), Napi::Number::New(env, m_OpenTime));
  obj.Set(Napi::String::New(env, "pl"), Napi::Number::New(env, m_PL));
  obj.Set(Napi::String::New(env, "net_pl"), Napi::Number::New(env, m_NetPL));
  obj.Set(Napi::String::New(env, "gross_pl"), Napi::Number::New(env, m_GrossPL));
  obj.Set(Napi::String::New(env, "commission"), Napi::Number::New(env, m_Commission));
  obj.Set(Napi::String::New(env, "rollover_interest"), Napi::Number::New(env, m_RolloverInterest));
  obj.Set(Napi::String::New(env, "open_quote_id"), Napi::String::New(env, m_OpenQuoteID));
  obj.Set(Napi::String::New(env, "open_order_id"), Napi::String::New(env, m_OpenOrderID));  
  obj.Set(Napi::String::New(env, "open_order_req_id"), Napi::String::New(env, m_OpenOrderReqID));
  obj.Set(Napi::String::New(env, "open_order_request_txt"), Napi::String::New(env, m_OpenOrderRequestTXT));
  obj.Set(Napi::String::New(env, "close_rate"), Napi::Number::New(env, m_CloseRate));
  obj.Set(Napi::String::New(env, "close_time"), Napi::Number::New(env, m_CloseTime));
  obj.Set(Napi::String::New(env, "close_quote_id"), Napi::String::New(env, m_CloseQuoteID));
  obj.Set(Napi::String::New(env, "close_order_id"), Napi::String::New(env, m_CloseOrderID));  
  obj.Set(Napi::String::New(env, "close_order_req_id"), Napi::String::New(env, m_CloseOrderReqID));
  obj.Set(Napi::String::New(env, "close_order_request_txt"), Napi::String::New(env, m_CloseOrderRequestTXT));
  obj.Set(Napi::String::New(env, "trade_id_origin"), Napi::String::New(env, m_TradeIDOrigin));
  obj.Set(Napi::String::New(env, "trade_id_remain"), Napi::String::New(env, m_TradeIDRemain));
  obj.Set(Napi::String::New(env, "value_date"), Napi::String::New(env, m_ValueDate));
  obj.Set(Napi::String::New(env, "dividends"), Napi::Number::New(env, m_Dividends));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
