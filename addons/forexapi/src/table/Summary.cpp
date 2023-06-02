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
#include "Summary.h"

using namespace fxdaemon;

Summary::Summary(Status status) : Table(status)
{
  m_SellNetPL = 0;
  m_SellNetPLPip = 0;
  m_SellAmount = 0;
  m_SellAvgOpen = 0;
  m_BuyClose = 0;
  m_SellClose = 0;
  m_BuyAvgOpen = 0;
  m_BuyAmount = 0;
  m_BuyNetPL = 0;
  m_BuyNetPLPip = 0;
  m_Amount = 0;
  m_GrossPL = 0;
  m_NetPL = 0;
  m_RolloverInterestSum = 0;
  m_UsedMargin = 0;
  m_UsedMarginBuy = 0;
  m_UsedMarginSell = 0;
  m_Commission = 0;
  m_CloseCommission = 0;
  m_Dividends = 0;
}

const std::string &Summary::getOfferID() const
{
  return m_OfferID;
}

void Summary::setOfferID(const char *offerID)
{
  m_OfferID = offerID;
}

double Summary::getSellNetPL() const
{
  return m_SellNetPL;
}

void Summary::setSellNetPL(double sellNetPL)
{
  m_SellNetPL = sellNetPL;
}

double Summary::getSellNetPLPip() const
{
  return m_SellNetPLPip;
}

void Summary::setSellNetPLPip(double sellNetPLPip)
{
  m_SellNetPLPip = sellNetPLPip;
}

double Summary::getSellAmount() const
{
  return m_SellAmount;
}

void Summary::setSellAmount(double sellAmount)
{
  m_SellAmount = sellAmount;
}

double Summary::getSellAvgOpen() const
{
  return m_SellAvgOpen;
}

void Summary::setSellAvgOpen(double sellAvgOpen)
{
  m_SellAvgOpen = sellAvgOpen;
}

double Summary::getBuyClose() const
{
  return m_BuyClose;
}

void Summary::setBuyClose(double buyClose)
{
  m_BuyClose = buyClose;
}

double Summary::getSellClose() const
{
  return m_SellClose;
}

void Summary::setSellClose(double sellClose)
{
  m_SellClose = sellClose;
}

double Summary::getBuyAvgOpen() const
{
  return m_BuyAvgOpen;
}

void Summary::setBuyAvgOpen(double buyAvgOpen)
{
  m_BuyAvgOpen = buyAvgOpen;
}

double Summary::getBuyAmount() const
{
  return m_BuyAmount;
}

void Summary::setBuyAmount(double buyAmount)
{
  m_BuyAmount = buyAmount;
}

double Summary::getBuyNetPL() const
{
  return m_BuyNetPL;
}

void Summary::setBuyNetPL(double buyNetPL)
{
  m_BuyNetPL = buyNetPL;
}

double Summary::getBuyNetPLPip() const
{
  return m_BuyNetPLPip;
}

void Summary::setBuyNetPLPip(double buyNetPLPip)
{
  m_BuyNetPLPip = buyNetPLPip;
}

double Summary::getAmount() const
{
  return m_Amount;
}

void Summary::setAmount(double amount)
{
  m_Amount = amount;
}

double Summary::getGrossPL() const
{
  return m_GrossPL;
}

void Summary::setGrossPL(double grossPL)
{
  m_GrossPL = grossPL;
}

double Summary::getNetPL() const
{
  return m_NetPL;
}

void Summary::setNetPL(double netPL)
{
  m_NetPL = netPL;
}

double Summary::getRolloverInterestSum() const
{
  return m_RolloverInterestSum;
}

void Summary::setRolloverInterestSum(double rolloverInterestSum)
{
  m_RolloverInterestSum = rolloverInterestSum;
}

double Summary::getUsedMargin() const
{
  return m_UsedMargin;
}

void Summary::setUsedMargin(double usedMargin)
{
  m_UsedMargin = usedMargin;
}

double Summary::getUsedMarginBuy() const
{
  return m_UsedMarginBuy;
}

void Summary::setUsedMarginBuy(double usedMarginBuy)
{
  m_UsedMarginBuy = usedMarginBuy;
}

double Summary::getUsedMarginSell() const
{
  return m_UsedMarginSell;
}

void Summary::setUsedMarginSell(double usedMarginSell)
{
  m_UsedMarginSell = usedMarginSell;
}

double Summary::getCommission() const
{
  return m_Commission;
}

void Summary::setCommission(double commission)
{
  m_Commission = commission;
}

double Summary::getCloseCommission() const
{
  return m_CloseCommission;
}

void Summary::setCloseCommission(double closeCommission)
{
  m_CloseCommission = closeCommission;
}

double Summary::getDividends() const
{
  return m_Dividends;
}

void Summary::setDividends(double dividends)
{
  m_Dividends = dividends;
}

Napi::Value Summary::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "offer_id"), Napi::String::New(env, m_OfferID));
  obj.Set(Napi::String::New(env, "sell_net_pl"), Napi::Number::New(env, m_SellNetPL));
  obj.Set(Napi::String::New(env, "sell_net_pl_pip"), Napi::Number::New(env, m_SellNetPLPip));
  obj.Set(Napi::String::New(env, "sell_amount"), Napi::Number::New(env, m_SellAmount));
  obj.Set(Napi::String::New(env, "sell_avg_open"), Napi::Number::New(env, m_SellAvgOpen));
  obj.Set(Napi::String::New(env, "buy_close"), Napi::Number::New(env, m_BuyClose));
  obj.Set(Napi::String::New(env, "sell_close"), Napi::Number::New(env, m_SellClose));
  obj.Set(Napi::String::New(env, "buy_avg_open"), Napi::Number::New(env, m_BuyAvgOpen));
  obj.Set(Napi::String::New(env, "buy_amount"), Napi::Number::New(env, m_BuyAmount));
  obj.Set(Napi::String::New(env, "buy_net_pl"), Napi::Number::New(env, m_BuyNetPL));
  obj.Set(Napi::String::New(env, "buy_net_pl_pip"), Napi::Number::New(env, m_BuyNetPLPip));
  obj.Set(Napi::String::New(env, "amount"), Napi::Number::New(env, m_Amount));
  obj.Set(Napi::String::New(env, "gross_pl"), Napi::Number::New(env, m_GrossPL));
  obj.Set(Napi::String::New(env, "net_pl"), Napi::Number::New(env, m_NetPL));
  obj.Set(Napi::String::New(env, "rollover_interest_sum"), Napi::Number::New(env, m_RolloverInterestSum));
  obj.Set(Napi::String::New(env, "used_margin"), Napi::Number::New(env, m_UsedMargin));
  obj.Set(Napi::String::New(env, "used_margin_buy"), Napi::Number::New(env, m_UsedMarginBuy));
  obj.Set(Napi::String::New(env, "used_margin_sell"), Napi::Number::New(env, m_UsedMarginSell));
  obj.Set(Napi::String::New(env, "commission"), Napi::Number::New(env, m_Commission));
  obj.Set(Napi::String::New(env, "close_commission"), Napi::Number::New(env, m_CloseCommission));
  obj.Set(Napi::String::New(env, "dividends"), Napi::Number::New(env, m_Dividends));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
