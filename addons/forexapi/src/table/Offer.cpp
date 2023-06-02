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
#include "Offer.h"

using namespace fxdaemon;

Offer::Offer(Status status) : Table(status)
{
  init();
}

Offer::Offer(const char *symbol) : Table(CHANGED)
{
  m_Symbol = symbol;
  init();
}

const std::string &Offer::getOfferID() const
{
  return m_OfferID;
}

void Offer::setOfferID(const char *offerID)
{
  m_OfferID = offerID;
}

const std::string &Offer::getSymbol() const
{
  return m_Symbol;
}

void Offer::setSymbol(const char *symbol)
{
  m_Symbol = symbol;
}

double Offer::getBid() const
{
  return m_Bid;
}

void Offer::setBid(double bid)
{
  m_Bid = bid;
}

double Offer::getAsk() const
{
  return m_Ask;
}

void Offer::setAsk(double ask)
{
  m_Ask = ask;
}

double Offer::getHigh() const
{
  return m_High;
}

void Offer::setHigh(double high)
{
  m_High = high;
}

double Offer::getLow() const
{
  return m_Low;
}

void Offer::setLow(double low)
{
  m_Low = low;
}

int Offer::getVolume() const
{
  return m_Volume;
}

void Offer::setVolume(int volume)
{
  m_Volume = volume;
}

time_t Offer::getTime() const
{
  return m_Time;
}

void Offer::setTime(time_t time)
{
  m_Time = time;
}

double Offer::getSellInterest() const
{
  return m_SellInterest;
}

void Offer::setSellInterest(double sellInterest)
{
  m_SellInterest = sellInterest;
}

double Offer::getBuyInterest() const
{
  return m_BuyInterest;
}

void Offer::setBuyInterest(double buyInterest)
{
  m_BuyInterest = buyInterest;
}

const std::string &Offer::getContractCurrency() const
{
  return m_ContractCurrency;
}

void Offer::setContractCurrency(const char *contractCurrency)
{
  m_ContractCurrency = contractCurrency;
}

int Offer::getDigits() const
{
  return m_Digits;
}

void Offer::setDigits(int digits)
{
  m_Digits = digits;
}

double Offer::getPipCost() const
{
  return m_PipCost;
}

void Offer::setPipCost(double pipCost)
{
  m_PipCost = pipCost;
}

double Offer::getPointSize() const
{
  return m_PointSize;
}

void Offer::setPointSize(double pointSize)
{
  m_PointSize = pointSize;
}

const std::string &Offer::getInstrumentType() const
{
  return m_InstrumentType;
}

void Offer::setInstrumentType(const char *instrumentType)
{
  m_InstrumentType = instrumentType;
}

double Offer::getContractMultiplier() const
{
  return m_ContractMultiplier;
}

void Offer::setContractMultiplier(double contractMultiplier)
{
  m_ContractMultiplier = contractMultiplier;
}

const std::string &Offer::getValueDate() const
{
  return m_ValueDate;
}

void Offer::setValueDate(const char *valueDate)
{
  m_ValueDate = valueDate;
}

void Offer::update(Offer *offer)
{
  if (offer->getBid() > 0) {
    m_Bid = offer->getBid();
  }
  if (offer->getAsk() > 0) {
    m_Ask = offer->getAsk();
  }
  if (offer->getHigh() > 0) {
    m_High = offer->getHigh();
  }
  if (offer->getLow() > 0) {
    m_Low = offer->getLow();
  }
  if (offer->getVolume() > 0) {
    m_Volume = offer->getVolume();
  }
  if (offer->getTime() > 0) {
    m_Time = offer->getTime();
  }
}

Napi::Value Offer::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "offer_id"), Napi::String::New(env, m_OfferID));
  obj.Set(Napi::String::New(env, "symbol"), Napi::String::New(env, m_Symbol));
  obj.Set(Napi::String::New(env, "bid"), Napi::Number::New(env, m_Bid));
  obj.Set(Napi::String::New(env, "ask"), Napi::Number::New(env, m_Ask));
  obj.Set(Napi::String::New(env, "high"), Napi::Number::New(env, m_High));
  obj.Set(Napi::String::New(env, "low"), Napi::Number::New(env, m_Low));
  obj.Set(Napi::String::New(env, "volume"), Napi::Number::New(env, m_Volume));
  obj.Set(Napi::String::New(env, "time"), Napi::Number::New(env, m_Time));
  obj.Set(Napi::String::New(env, "sell_interest"), Napi::Number::New(env, m_SellInterest));
  obj.Set(Napi::String::New(env, "buy_interest"), Napi::Number::New(env, m_BuyInterest));
  obj.Set(Napi::String::New(env, "contract_currency"), Napi::String::New(env, m_ContractCurrency));
  obj.Set(Napi::String::New(env, "digits"), Napi::Number::New(env, m_Digits));
  obj.Set(Napi::String::New(env, "pip_cost"), Napi::Number::New(env, m_PipCost));
  obj.Set(Napi::String::New(env, "point_size"), Napi::Number::New(env, m_PointSize));
  obj.Set(Napi::String::New(env, "symbol_type"), Napi::String::New(env, m_InstrumentType));
  obj.Set(Napi::String::New(env, "contract_multiplier"), Napi::Number::New(env, m_ContractMultiplier));
  obj.Set(Napi::String::New(env, "value_date"), Napi::String::New(env, m_ValueDate));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}

void Offer::init()
{
  m_Bid = 0;
  m_Ask = 0;
  m_High = 0;
  m_Low = 0;
  m_Volume = 0;
  m_Time = 0;
  m_SellInterest = 0;
  m_BuyInterest = 0;
  m_Digits = 1;
  m_PipCost = 1;
  m_PointSize = 1;
  m_ContractMultiplier = 0;
}
