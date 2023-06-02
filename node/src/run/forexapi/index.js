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

const { setGlobals } = require('../../global/global.js');
const dataset = require('../../storage/dataset.js');
const serializer = require('../../storage/serialize.js');
const { toArray } = require('../../utils/helper.js');
const ForexApi = require('./addon.js');
const { handleEvent } = require('./events.js');

let forexApi;

function getServerTime() {
  return forexApi.node.getServerTime();
}

function getEndTradingDay() {
  return forexApi.node.getEndTradingDay();
}

function getHistoricalPrices(symbol, period, startTime, endTime) {
  return new Promise((resolve, reject) => {
    forexApi.node.getHistoricalPricesAsync(
      symbol,
      period,
      startTime,
      endTime,
      ({ data, error }) => {
        if (error) {
          reject(new Error(error));
        } else {
          resolve(data);
        }
      },
    );
  });
}

function openPosition(accountID, symbol, bs, amount, stop = 0, limit = 0) {
  return forexApi.node.openPosition(accountID, symbol, bs, amount, stop, limit);
}

function changeStopLoss(accountID, tradeID, stop, amount = 0) {
  return forexApi.node.changeStopLoss(accountID, tradeID, amount, stop);
}

function changeTakeProfit(accountID, tradeID, limit, amount = 0) {
  return forexApi.node.changeTakeProfit(accountID, tradeID, amount, limit);
}

function closePosition(accountID, tradeID, amount = 0) {
  return forexApi.node.closePosition(accountID, tradeID, amount);
}

function closeAllPositions(accountID, symbol = '') {
  return forexApi.node.closeAllPositions(accountID, symbol);
}

function getOffer(symbol) {
  return dataset.getOffers()[symbol];
}

function getPrices(symbol, period, priceTypes = []) {
  const prices = new Array(priceTypes.length).fill([]);
  const candles = getCandles(symbol, period);
  candles &&
    priceTypes.forEach((priceType, i) => {
      prices[i] = candles.map(candle => candle[priceType] || 0);
    });
  return prices;
}

function getCandles(symbol, period) {
  return dataset.getCandles(symbol, period);
}

function getAccounts() {
  return dataset.getAccounts();
}

function getOrders() {
  return dataset.getOrders();
}

function getPositions() {
  return dataset.getPositions();
}

function getClosedPositions() {
  return dataset.getClosedPositions();
}

function getSummary() {
  return dataset.getSummary();
}

async function save() {
  const offers = dataset.getOffers();
  for (const id in offers) {
    await serializer.insertOnConflict(offers[id]);
  }
  for (const account of getAccounts()) {
    await serializer.insertOnConflict(account);
  }
  for (const order of getOrders()) {
    await serializer.insertOnConflict(order);
  }
  for (const position of getPositions()) {
    await serializer.insertOnConflict(position);
  }
  for (const closedPositions of getClosedPositions()) {
    await serializer.insertOnConflict(closedPositions);
  }
  return true;
}

function init(params) {
  const { userName, password, host, connection, symbols, periods } = params;
  forexApi = new ForexApi({
    userName,
    password,
    host,
    connection,
    symbols: toArray(symbols),
    periods: toArray(periods),
  });

  setGlobals({
    getServerTime,
    getEndTradingDay,
    getHistoricalPrices,
    openPosition,
    changeStopLoss,
    changeTakeProfit,
    closePosition,
    closeAllPositions,
    getOffer,
    getPrices,
    getCandles,
    getAccounts,
    getOrders,
    getPositions,
    getClosedPositions,
    getSummary,
  });
}

function start(handleError) {
  forexApi
    .start()
    .then(data => {
      _.logger.debug(data);
      dataset.setOffers(data.offers);
      dataset.setCandles(data.candles);
      dataset.setAccounts(data.accounts);
      dataset.setOrders(data.orders);
      dataset.setPositions(data.trades);
      dataset.setClosedPositions(data.closed_trades);
      dataset.setSummary(data.summary);
      save().then(() => __CB__.init());
    })
    .catch(err => {
      _.logger.error(err);
      handleError(err);
    });

  forexApi.on(event => {
    const { type, data } = event;
    handleEvent(type, data);
  });
}

function stop() {
  return new Promise(resolve => {
    __CB__.exit();
    save().then(() => {
      forexApi.stop();
      resolve();
    });
  });
}

module.exports = { init, start, stop };
