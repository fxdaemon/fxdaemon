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
#include "Order2Go.h"

static const char *ErrArgumentWrongNumber = "Wrong number of arguments";
static const char *ErrObjectExpected = "Object expected";

static Order2Go order2Go;

// in:  { session_id, pin }
Napi::Value init(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() > 0 && !info[0].IsObject()) {
    Napi::TypeError::New(env, ErrObjectExpected).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string sessionID;
  std::string pin;
  if (info.Length() > 0) {
    Napi::Object obj = info[0].ToObject();
    if (obj.Has("session_id")) {
      sessionID = obj.Get(static_cast<napi_value>(Napi::String::New(env, "session_id"))).As<Napi::String>();
    }
    if (obj.Has("pin")) {
      pin = obj.Get(static_cast<napi_value>(Napi::String::New(env, "pin"))).As<Napi::String>();
    }
  }
  std::string error;
  bool completed = order2Go.init(sessionID.c_str(), pin.c_str(), error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

// in:  userName, password, host, connection
Napi::Value login(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 4) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string userName = info[0].As<Napi::String>().ToString();
  std::string password = info[1].As<Napi::String>().ToString();
  std::string host = info[2].As<Napi::String>().ToString();
  std::string connection = info[3].As<Napi::String>().ToString();
  std::string error;
  bool connected = order2Go.login(userName.c_str(), password.c_str(), host.c_str(), connection.c_str(), error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "connected"), Napi::Boolean::New(env, connected));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

Napi::Value logout(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Boolean::New(env, order2Go.logout());
}

Napi::Value release(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  order2Go.close();
  return env.Null();
}

bool preLoadData(const Napi::CallbackInfo &info, SymbolsType &symbols, 
                 PeriodsType &periods, int *priceHistoryCount)
{
  Napi::Env env = info.Env();
  if (info.Length() != 2) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return false;
  }

  Napi::Object obj = info[0].ToObject();
  auto inarrayToSet = [&](const char *name, std::set<std::string> &sets) {
    if (obj.Has(name)) {
      Napi::Value value = obj.Get(static_cast<napi_value>(Napi::String::New(env, name)));
      if (value.IsArray()) {
        Napi::Array array = value.As<Napi::Array>();
        for (uint32_t i = 0; i < array.Length(); i++) {
          sets.insert(static_cast<Napi::Value>(array[i]).As<Napi::String>().ToString());
        }
      }
    }
  };

  inarrayToSet("symbols", symbols);
  inarrayToSet("periods", periods);
  if (obj.Has("priceHistoryCount")) {
    *priceHistoryCount = obj.Get(static_cast<napi_value>(
      Napi::String::New(env, "priceHistoryCount"))).As<Napi::Number>();
  }
  return true;
}

// in:  { symbols, periods, priceHistoryCount }, callback
Napi::Value loadData(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  SymbolsType symbols;
  PeriodsType periods;
  int priceHistoryCount = 0;
  if (preLoadData(info, symbols, periods, &priceHistoryCount)) {
    order2Go.loadData(symbols, periods, priceHistoryCount);
    Napi::Function callback = info[1].As<Napi::Function>();
    callback.Call(env.Global(), {order2Go.valueOf(env)});
  }
  return env.Null();
}

// in:  { symbols, periods, priceHistoryCount }, callback
Napi::Value loadDataAsync(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  SymbolsType symbols;
  PeriodsType periods;
  int priceHistoryCount = 0;
  if (preLoadData(info, symbols, periods, &priceHistoryCount)) {
    order2Go.loadData(symbols, periods, priceHistoryCount, info[1].As<Napi::Function>());
  }
  return env.Null();
}

Napi::Value getServerTime(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), order2Go.getServerTime());
}

Napi::Value getEndTradingDay(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), order2Go.getEndTradingDay());
}

// in:  symbol, period, startTime, endTime, callback
Napi::Value getHistoricalPricesAsync(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 5) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string symbol = info[0].As<Napi::String>().ToString();
  std::string period = info[1].As<Napi::String>().ToString();
  time_t startTime = info[2].As<Napi::Number>().ToNumber();
  time_t endTime = info[3].As<Napi::Number>().ToNumber();
  Napi::Function callback = info[4].As<Napi::Function>();
  order2Go.getHistoricalPrices(symbol, period, startTime, endTime, callback);
  return env.Null();
}

// in:  accountID, symbol, bs, amount, stop, limit
Napi::Value openPosition(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 6) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string accountID = info[0].As<Napi::String>().ToString();
  std::string symbol = info[1].As<Napi::String>().ToString();
  std::string bs = info[2].As<Napi::String>().ToString();
  double amount = info[3].As<Napi::Number>().ToNumber();
  double stop = info[4].As<Napi::Number>().ToNumber();
  double limit = info[5].As<Napi::Number>().ToNumber();

  std::string orderID;
  std::string error;
  bool completed = order2Go.openPosition(accountID, symbol, bs, amount, stop, limit, orderID, error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "order_id"), Napi::String::New(env, orderID));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

// in:  accountID, tradeID, amount, stop
Napi::Value changeStopLoss(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 4) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string accountID = info[0].As<Napi::String>().ToString();
  std::string tradeID = info[1].As<Napi::String>().ToString();
  double amount = info[2].As<Napi::Number>().ToNumber();
  double stop = info[3].As<Napi::Number>().ToNumber();

  std::string requestID;
  std::string error;
  bool completed = order2Go.changeStopLoss(accountID, tradeID, amount, stop, requestID, error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "request_id"), Napi::String::New(env, requestID));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

// in:  accountID, tradeID, amount, limit
Napi::Value changeTakeProfit(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 4) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string accountID = info[0].As<Napi::String>().ToString();
  std::string tradeID = info[1].As<Napi::String>().ToString();
  double amount = info[2].As<Napi::Number>().ToNumber();
  double limit = info[3].As<Napi::Number>().ToNumber();

  std::string requestID;
  std::string error;
  bool completed = order2Go.changeTakeProfit(accountID, tradeID, amount, limit, requestID, error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "request_id"), Napi::String::New(env, requestID));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

// in:  accountID, tradeID, amount
Napi::Value closePosition(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 3) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string accountID = info[0].As<Napi::String>().ToString();
  std::string tradeID = info[1].As<Napi::String>().ToString();
  double amount = info[2].As<Napi::Number>().ToNumber();

  std::string requestID;
  std::string error;
  bool completed = order2Go.closePosition(accountID, tradeID, amount, requestID, error);

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "request_id"), Napi::String::New(env, requestID));
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

// in:  accountID, symbol
Napi::Value closeAllPositions(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 2) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string accountID = info[0].As<Napi::String>().ToString();
  std::string symbol = info[1].As<Napi::String>().ToString();

  std::vector<std::string> requestIDs;
  std::string error;
  bool completed = order2Go.closeAllPositions(accountID, symbol, requestIDs, error);

  Napi::Array requestIDArray = Napi::Array::New(env, requestIDs.size());
  for (uint32_t i = 0; i < requestIDs.size(); i++) {
    requestIDArray[i] = Napi::String::New(env, requestIDs[i]);
  }

  Napi::Object ret = Napi::Object::New(env);
  ret.Set(Napi::String::New(env, "completed"), Napi::Boolean::New(env, completed));
  ret.Set(Napi::String::New(env, "request_ids"), requestIDArray);
  ret.Set(Napi::String::New(env, "error"), Napi::String::New(env, error));
  return ret;
}

Napi::Value waitEvents(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Boolean::New(env, order2Go.waitEvents());
}

// in:  callback
Napi::Value nextEvent(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 1) {
    Napi::TypeError::New(env, ErrArgumentWrongNumber).ThrowAsJavaScriptException();
    return env.Null();
  }

  Event event = order2Go.popEvent();
  if (event.isAvailable()) {
    Napi::Object arguments = Napi::Object::New(env);
    arguments.Set(Napi::String::New(env, "type"), Napi::String::New(env, event.getTypeText()));
    arguments.Set(Napi::String::New(env, "data"), event.data->toValue(env));
    Napi::Function callback = info[0].As<Napi::Function>();
    callback.Call(env.Global(), {arguments});
  }
  return Napi::Boolean::New(env, event.isAvailable());
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, init));
  exports.Set(Napi::String::New(env, "login"), Napi::Function::New(env, login));
  exports.Set(Napi::String::New(env, "logout"), Napi::Function::New(env, logout));
  exports.Set(Napi::String::New(env, "close"), Napi::Function::New(env, release));
  exports.Set(Napi::String::New(env, "loadData"), Napi::Function::New(env, loadData));
  exports.Set(Napi::String::New(env, "loadDataAsync"), Napi::Function::New(env, loadDataAsync));
  exports.Set(Napi::String::New(env, "getServerTime"), Napi::Function::New(env, getServerTime));
  exports.Set(Napi::String::New(env, "getEndTradingDay"), Napi::Function::New(env, getEndTradingDay));
  exports.Set(Napi::String::New(env, "getHistoricalPricesAsync"), Napi::Function::New(env, getHistoricalPricesAsync));
  exports.Set(Napi::String::New(env, "openPosition"), Napi::Function::New(env, openPosition));
  exports.Set(Napi::String::New(env, "changeStopLoss"), Napi::Function::New(env, changeStopLoss));
  exports.Set(Napi::String::New(env, "changeTakeProfit"), Napi::Function::New(env, changeTakeProfit));
  exports.Set(Napi::String::New(env, "closePosition"), Napi::Function::New(env, closePosition));
  exports.Set(Napi::String::New(env, "closeAllPositions"), Napi::Function::New(env, closeAllPositions));
  exports.Set(Napi::String::New(env, "waitEvents"), Napi::Function::New(env, waitEvents));
  exports.Set(Napi::String::New(env, "nextEvent"), Napi::Function::New(env, nextEvent));
  return exports;
}

NODE_API_MODULE(forexapi, Init)
