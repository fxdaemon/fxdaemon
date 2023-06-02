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
#include "Order.h"

using namespace fxdaemon;

Order::Order(Status status) : Table(status)
{
  m_NetQuantity = false;
  m_Amount = 0;
	m_Rate = 0;
  m_ExecutionRate = 0;
  m_RateMin = 0;
  m_RateMax = 0;
	m_Stop = 0;
	m_Limit = 0;
	m_StatusTime = 0;
  m_AtMarket = 0;
  m_TrailStep = 0;
  m_TrailRate = 0;
  m_OriginAmount = 0;
  m_FilledAmount = 0;
  m_WorkingIndicator = false;
  m_PegOffset = 0;
  m_ExpireTime = 0;
}

const std::string &Order::getOrderID() const
{
  return m_OrderID;
}

void Order::setOrderID(const char *orderID)
{
  m_OrderID = orderID;
}

const std::string &Order::getRequestID() const
{
  return m_RequestID;
}

void Order::setRequestID(const char *requestID)
{
  m_RequestID = requestID;
}

const std::string &Order::getAccountID() const
{
  return m_AccountID;
}

void Order::setAccountID(const char *accountID)
{
  m_AccountID = accountID;
}

const std::string &Order::getOfferID() const
{
  return m_OfferID;
}

void Order::setOfferID(const char *offerID)
{
  m_OfferID = offerID;
}

const std::string &Order::getTradeID() const
{
  return m_TradeID;
}

void Order::setTradeID(const char *tradeID)
{
  m_TradeID = tradeID;
}

bool Order::getNetQuantity() const
{
  return m_NetQuantity;
}

void Order::setNetQuantity(bool netQuantity)
{
  m_NetQuantity = netQuantity;
}

const std::string &Order::getBS() const
{
  return m_BS;
}

void Order::setBS(const char *bs)
{
  m_BS = bs;
}

const std::string &Order::getStage() const
{
  return m_Stage;
}

void Order::setStage(const char *stage)
{
  m_Stage = stage;
}

const std::string &Order::getType() const
{
  return m_Type;
}

void Order::setType(const char *type)
{
  m_Type = type;
}

const std::string &Order::getOrderStatus() const
{
  return m_OrderStatus;
}

void Order::setOrderStatus(const char *orderStatus)
{
  m_OrderStatus = orderStatus;
}

double Order::getAmount() const
{
  return m_Amount;
}

void Order::setAmount(double amount)
{
  m_Amount = amount;
}

double Order::getRate() const
{
  return m_Rate;
}

void Order::setRate(double rate)
{
  m_Rate = rate;
}

double Order::getExecutionRate() const
{
  return m_ExecutionRate;
}

void Order::setExecutionRate(double executionRate)
{
  m_ExecutionRate = executionRate;
}

double Order::getRateMin() const
{
  return m_RateMin;
}

void Order::setRateMin(double rateMin)
{
  m_RateMin = rateMin;
}

double Order::getRateMax() const
{
  return m_RateMax;
}

void Order::setRateMax(double rateMax)
{
  m_RateMax = rateMax;
}

double Order::getStop() const
{
  return m_Stop;
}

void Order::setStop(double stop)
{
  m_Stop = stop;
}

double Order::getLimit() const
{
  return m_Limit;
}

void Order::setLimit(double limit)
{
  m_Limit = limit;
}

const std::string &Order::getTypeStop() const
{
  return m_TypeStop;
}

void Order::setTypeStop(const char *typeStop)
{
  m_TypeStop = typeStop;
}

const std::string &Order::getTypeLimit() const
{
  return m_TypeLimit;
}

void Order::setTypeLimit(const char *typeLimit)
{
  m_TypeLimit = typeLimit;
}

time_t Order::getStatusTime() const
{
  return m_StatusTime;
}

void Order::setStatusTime(time_t statusTime)
{
  m_StatusTime = statusTime;
}

double Order::getAtMarket() const
{
  return m_AtMarket;
}

void Order::setAtMarket(double atMarket)
{
  m_AtMarket = atMarket;
}

int Order::getTrailStep() const
{
  return m_TrailStep;
}

void Order::setTrailStep(int trailStep)
{
  m_TrailStep = trailStep;
}

double Order::getTrailRate() const
{
  return m_TrailRate;
}

void Order::setTrailRate(double trailRate)
{
  m_TrailRate = trailRate;
}

const std::string &Order::getTimeInForce() const
{
  return m_TimeInForce;
}

void Order::setTimeInForce(const char *timeInForce)
{
  m_TimeInForce = timeInForce;
}

const std::string &Order::getContingentOrderID() const
{
  return m_ContingentOrderID;
}

void Order::setContingentOrderID(const char *contingentOrderID)
{
  m_ContingentOrderID = contingentOrderID;
}

const std::string &Order::getContingencyType() const
{
  return m_ContingencyType;
}

void Order::setContingencyType(const char *contingencyType)
{
  m_ContingencyType = contingencyType;
}

const std::string &Order::getPrimaryID() const
{
  return m_PrimaryID;
}

void Order::setPrimaryID(const char *primaryID)
{
  m_PrimaryID = primaryID;
}

double Order::getOriginAmount() const
{
  return m_OriginAmount;
}

void Order::setOriginAmount(double originAmount)
{
  m_OriginAmount = originAmount;
}

double Order::getFilledAmount() const
{
  return m_FilledAmount;
}
  
void Order::setFilledAmount(double filledAmount)
{
  m_FilledAmount = filledAmount;
}

bool Order::getWorkingIndicator() const
{
  return m_WorkingIndicator;
}

void Order::setWorkingIndicator(bool workingIndicator)
{
  m_WorkingIndicator = workingIndicator;
}

const std::string &Order::getPegType() const
{
  return m_PegType;
}

void Order::setPegType(const char *pegType)
{
  m_PegType = pegType;
}

double Order::getPegOffset() const
{
  return m_PegOffset;
}

void Order::setPegOffset(double pegOffset)
{
  m_PegOffset = pegOffset;

}

time_t Order::getExpireTime() const
{
  return m_ExpireTime;
}

void Order::setExpireTime(time_t expireTime)
{
  m_ExpireTime = expireTime;
}

const std::string &Order::getValueDate() const
{
  return m_ValueDate;
}

void Order::setValueDate(const char *valueDate)
{
  m_ValueDate = valueDate;
}

Napi::Value Order::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "order_id"), Napi::String::New(env, m_OrderID));
  obj.Set(Napi::String::New(env, "request_id"), Napi::String::New(env, m_RequestID));
  obj.Set(Napi::String::New(env, "account_id"), Napi::String::New(env, m_AccountID));
  obj.Set(Napi::String::New(env, "offer_id"), Napi::String::New(env, m_OfferID));
  obj.Set(Napi::String::New(env, "trade_id"), Napi::String::New(env, m_TradeID));
  obj.Set(Napi::String::New(env, "net_quantity"), Napi::Boolean::New(env, m_NetQuantity));
  obj.Set(Napi::String::New(env, "side"), Napi::String::New(env, m_BS));
  obj.Set(Napi::String::New(env, "stage"), Napi::String::New(env, m_Stage));
  obj.Set(Napi::String::New(env, "type"), Napi::String::New(env, m_Type));
  obj.Set(Napi::String::New(env, "order_status"), Napi::String::New(env, m_OrderStatus));
  obj.Set(Napi::String::New(env, "amount"), Napi::Number::New(env, m_Amount));
  obj.Set(Napi::String::New(env, "rate"), Napi::Number::New(env, m_Rate));
  obj.Set(Napi::String::New(env, "execution_rate"), Napi::Number::New(env, m_ExecutionRate));
  obj.Set(Napi::String::New(env, "rate_min"), Napi::Number::New(env, m_RateMin));
  obj.Set(Napi::String::New(env, "rate_max"), Napi::Number::New(env, m_RateMax));
  obj.Set(Napi::String::New(env, "stop"), Napi::Number::New(env, m_Stop));
  obj.Set(Napi::String::New(env, "limit"), Napi::Number::New(env, m_Limit));
  obj.Set(Napi::String::New(env, "type_stop"), Napi::String::New(env, m_TypeStop));
  obj.Set(Napi::String::New(env, "type_limit"), Napi::String::New(env, m_TypeLimit));
  obj.Set(Napi::String::New(env, "status_time"), Napi::Number::New(env, m_StatusTime));
  obj.Set(Napi::String::New(env, "at_market"), Napi::Number::New(env, m_AtMarket));
  obj.Set(Napi::String::New(env, "trail_step"), Napi::Number::New(env, m_TrailStep));
  obj.Set(Napi::String::New(env, "trail_rate"), Napi::Number::New(env, m_TrailRate));
  obj.Set(Napi::String::New(env, "time_in_force"), Napi::String::New(env, m_TimeInForce));
  obj.Set(Napi::String::New(env, "contingent_order_id"), Napi::String::New(env, m_ContingentOrderID));  
  obj.Set(Napi::String::New(env, "contingency_type"), Napi::String::New(env, m_ContingencyType));
  obj.Set(Napi::String::New(env, "primary_id"), Napi::String::New(env, m_PrimaryID));
  obj.Set(Napi::String::New(env, "origin_amount"), Napi::Number::New(env, m_OriginAmount));
  obj.Set(Napi::String::New(env, "filled_amount"), Napi::Number::New(env, m_FilledAmount));
  obj.Set(Napi::String::New(env, "working_indicator"), Napi::Boolean::New(env, m_WorkingIndicator));
  obj.Set(Napi::String::New(env, "peg_type"), Napi::String::New(env, m_PegType));
  obj.Set(Napi::String::New(env, "peg_offset"), Napi::Number::New(env, m_PegOffset));
  obj.Set(Napi::String::New(env, "expire_time"), Napi::Number::New(env, m_ExpireTime));
  obj.Set(Napi::String::New(env, "value_date"), Napi::String::New(env, m_ValueDate));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
