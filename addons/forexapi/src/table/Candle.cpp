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
#include "Candle.h"

using namespace fxdaemon;

Candle::Candle(Status status) : Table(status)
{
  m_StartTime = 0;
  m_AskOpen = 0;
  m_AskHigh = 0;
  m_AskLow = 0;
  m_AskClose = 0;
  m_BidOpen = 0;
  m_BidHigh = 0;
  m_BidLow = 0;
  m_BidClose = 0;
  m_Volume = 1;
}

Candle::Candle(const char *symbol, const char *period, time_t startTime,
              double ask, double bid, int volume) : Table(ADDED)
{
  m_Symbol = symbol;
  m_Period = period;
  m_StartTime = startTime;
  m_AskOpen = m_AskHigh = m_AskLow = m_AskClose = ask;
  m_BidOpen = m_BidHigh = m_BidLow = m_BidClose = bid;
  m_Volume = volume;
}

Candle::Candle(const char *symbol, const char *period, time_t startTime,
              double askOpen, double askHigh, double askLow, double askClose,
              double bidOpen, double bidHigh, double bidLow, double bidClose,
              int volume) : Table(ADDED)
{
  m_Symbol = symbol;
  m_Period = period;
  m_StartTime = startTime;
  m_AskOpen = askOpen;
  m_AskHigh = askHigh;
  m_AskLow = askLow;
  m_AskClose = askClose;
  m_BidOpen = bidOpen;
  m_BidHigh = bidHigh;
  m_BidLow = bidLow;
  m_BidClose = bidClose;
  m_Volume = volume;
}

const std::string &Candle::getSymbol() const
{
  return m_Symbol;
}

void Candle::setSymbol(const char *symbol)
{
  m_Symbol = symbol;
}

const std::string &Candle::getPeriod() const
{
  return m_Period;
}

void Candle::setPeriod(const char *period)
{
  m_Period = period;
}

time_t Candle::getStartTime() const
{
  return m_StartTime;
}

void Candle::setStartTime(time_t startTime)
{
  m_StartTime = startTime;
}

double Candle::getAskOpen() const
{
  return m_AskOpen;
}

void Candle::setAskOpen(double askOpen)
{
  m_AskOpen = askOpen;
}

double Candle::getAskHigh() const
{
  return m_AskHigh;
}

void Candle::setAskHigh(double askHigh)
{
  m_AskHigh = askHigh;
}

double Candle::getAskLow() const
{
  return m_AskLow;
}

void Candle::setAskLow(double askLow)
{
  m_AskLow = askLow;
}

double Candle::getAskClose() const
{
  return m_AskClose;
}

void Candle::setAskClose(double askClose)
{
  m_AskClose = askClose;
}

double Candle::getBidOpen() const
{
  return m_BidOpen;
}

void Candle::setBidOpen(double bidOpen)
{
  m_BidOpen = bidOpen;
}

double Candle::getBidHigh() const
{
  return m_BidHigh;
}

void Candle::setBidHigh(double bidHigh)
{
  m_BidHigh = bidHigh;
}

double Candle::getBidLow() const
{
  return m_BidLow;
}

void Candle::setBidLow(double bidLow)
{
  m_BidLow = bidLow;
}

double Candle::getBidClose() const
{
  return m_BidClose;
}

void Candle::setBidClose(double bidClose)
{
  m_BidClose = bidClose;
}

int Candle::getVolume() const
{
  return m_Volume;
}

void Candle::setVolume(int volume)
{
  m_Volume = volume;
}

void Candle::setAskHighLow(double ask)
{
  if (m_AskHigh < ask) {
    m_AskHigh = ask;
  }
  if (m_AskLow > ask) {
    m_AskLow = ask;
  }
}
  
void Candle::setBidHighLow(double bid)
{
  if (m_BidHigh < bid) {
    m_BidHigh = bid;
  }
  if (m_BidLow > bid) {
    m_BidLow = bid;
  }
}

Napi::Value Candle::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "symbol"), Napi::String::New(env, m_Symbol));
  obj.Set(Napi::String::New(env, "period"), Napi::String::New(env, m_Period));
  obj.Set(Napi::String::New(env, "start_time"), Napi::Number::New(env, m_StartTime));
  obj.Set(Napi::String::New(env, "ask_open"), Napi::Number::New(env, m_AskOpen));
  obj.Set(Napi::String::New(env, "ask_high"), Napi::Number::New(env, m_AskHigh));
  obj.Set(Napi::String::New(env, "ask_low"), Napi::Number::New(env, m_AskLow));
  obj.Set(Napi::String::New(env, "ask_close"), Napi::Number::New(env, m_AskClose));
  obj.Set(Napi::String::New(env, "ask_median"), Napi::Number::New(env, (m_AskHigh + m_AskLow) / 2));
  obj.Set(Napi::String::New(env, "ask_typical"), Napi::Number::New(env, (m_AskHigh + m_AskLow + m_AskClose) / 3));
  obj.Set(Napi::String::New(env, "ask_weighted"), Napi::Number::New(env, (m_AskHigh + m_AskLow + 2 * m_AskClose) / 4));
  obj.Set(Napi::String::New(env, "bid_open"), Napi::Number::New(env, m_BidOpen));
  obj.Set(Napi::String::New(env, "bid_high"), Napi::Number::New(env, m_BidHigh));
  obj.Set(Napi::String::New(env, "bid_low"), Napi::Number::New(env, m_BidLow));
  obj.Set(Napi::String::New(env, "bid_close"), Napi::Number::New(env, m_BidClose));
  obj.Set(Napi::String::New(env, "bid_median"), Napi::Number::New(env, (m_BidHigh + m_BidLow) / 2));
  obj.Set(Napi::String::New(env, "bid_typical"), Napi::Number::New(env, (m_BidHigh + m_BidLow + m_BidClose) / 3));
  obj.Set(Napi::String::New(env, "bid_weighted"), Napi::Number::New(env, (m_BidHigh + m_BidLow + 2 * m_BidClose) / 4));
  obj.Set(Napi::String::New(env, "volume"), Napi::Number::New(env, m_Volume));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
