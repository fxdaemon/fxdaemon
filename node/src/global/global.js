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

const defs = {
  config: null,
  emitter: null,
  logger: null,
  talib: null,
  TA: null,
  getServerTime: null,
  getEndTradingDay: null,
  getHistoricalPrices: null,
  openPosition: null,
  changeStopLoss: null,
  changeTakeProfit: null,
  closePosition: null,
  closeAllPositions: null,
  getOffer: null,
  getPrices: null,
  getCandles: null,
  getAccounts: null,
  getOrders: null,
  getPositions: null,
  getClosedPositions: null,
  getSummary: null,
};

global._ = defs;

function setGlobal(key, val) {
  defs[key] === null && (defs[key] = val);
}

function setGlobals(objs) {
  Object.keys(objs).forEach(key => setGlobal(key, objs[key]));
}

module.exports = { setGlobal, setGlobals };
