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

const { CANDLES_MAX_COUNT } = require('../global/constants.js');
const Offer = require('./entity/offer.js');
const Account = require('./entity/account.js');
const Order = require('./entity/order.js');
const Position = require('./entity/position.js');
const ClosedPosition = require('./entity/closed_position.js');

const offers = {};
const candlesSet = {};
const accounts = [];
const orders = [];
const positions = [];
const closedPositions = [];
const summary = [];

function setOffers(_offers, key = 'symbol') {
  _offers.forEach(offer => {
    offers[offer[key]] = new Offer(offer);
  });
}

function getOffers() {
  return offers;
}

function updateOffer(offer, key = 'symbol') {
  const lastOffer = offers[offer[key]];
  if (lastOffer) {
    Object.assign(lastOffer, offer);
    return lastOffer;
  }
}

function setCandles(_candlesSet) {
  Object.assign(candlesSet, _candlesSet);
}

function getCandles(symbol, period) {
  return candlesSet[symbol + period];
}

function addCandle(symbol, period, candle, timeCol = 'start_time') {
  const key = symbol + period;
  let candles = candlesSet[key];
  if (!candles) {
    candles = [];
    candlesSet[key] = candles;
  }
  if (candles.length) {
    const lastStartTime = candles[candles.length - 1][timeCol];
    const curStartTime = candle[timeCol];
    if (curStartTime < lastStartTime) {
      return;
    } else if (curStartTime === lastStartTime) {
      candles[candles.length - 1] = candle;
      return;
    }
  }
  if (candles.length >= CANDLES_MAX_COUNT) {
    candles.shift();
  }
  candles.push(candle);
}

function setAccounts(_accounts) {
  _accounts.forEach(account => {
    accounts.push(new Account(account));
  });
}

function getAccounts() {
  return accounts;
}

function updateAccount(account, key = 'account_id') {
  const accountID = account[key];
  for (let i = 0; i < accounts.length; i++) {
    if (accounts[i][key] === accountID) {
      const prevAccount = accounts[i];
      accounts[i] = new Account(account);
      return [prevAccount, accounts[i]];
    }
  }
}

function setOrders(_orders) {
  _orders.forEach(order => {
    orders.push(new Order(order));
  });
}

function getOrders() {
  return orders;
}

function addOrder(order) {
  orders.push(new Order(order));
  return orders[orders.length - 1];
}

function updateOrder(order, key = 'order_id') {
  const orderID = order[key];
  for (let i = 0; i < orders.length; i++) {
    if (orders[i][key] === orderID) {
      const prevOrder = orders[i];
      orders[i] = new Order(order);
      return [prevOrder, orders[i]];
    }
  }
}

function setPositions(_positions) {
  _positions.forEach(position => {
    positions.push(new Position(position));
  });
}

function getPositions() {
  return positions;
}

function addPosition(position) {
  positions.push(new Position(position));
  return positions[positions.length - 1];
}

function updatePosition(position, key = 'trade_id') {
  const tradeID = position[key];
  for (let i = 0; i < positions.length; i++) {
    if (positions[i][key] === tradeID) {
      const prevPosition = positions[i];
      positions[i] = new Position(position);
      return [prevPosition, positions[i]];
    }
  }
}

function deletePosition(position, key = 'trade_id') {
  const tradeID = position[key];
  for (let i = 0; i < positions.length; i++) {
    if (positions[i][key] === tradeID) {
      return positions.splice(i, 1)[0];
    }
  }
}

function setClosedPositions(_closedPositions) {
  _closedPositions.forEach(closedPosition => {
    closedPositions.push(new ClosedPosition(closedPosition));
  });
}

function getClosedPositions() {
  return closedPositions;
}

function addClosedPosition(closedPosition) {
  closedPositions.push(new ClosedPosition(closedPosition));
  return closedPositions[closedPositions.length - 1];
}

function setSummary(_summary) {
  summary.push(..._summary);
}

function getSummary() {
  return summary;
}

function addSummary(_summary) {
  summary.push(_summary);
}

function updateSummary(_summary, key = 'offer_id') {
  const offerID = _summary[key];
  for (let i = 0; i < summary.length; i++) {
    if (summary[i][key] === offerID) {
      const prevSummary = summary[i];
      summary[i] = _summary;
      return prevSummary;
    }
  }
}

function deleteSummary(_summary, key = 'offer_id') {
  const offerID = _summary[key];
  for (let i = 0; i < summary.length; i++) {
    if (summary[i][key] === offerID) {
      return summary.splice(i, 1)[0];
    }
  }
}

module.exports = {
  setOffers,
  getOffers,
  updateOffer,
  setCandles,
  getCandles,
  addCandle,
  setAccounts,
  getAccounts,
  updateAccount,
  setOrders,
  getOrders,
  addOrder,
  updateOrder,
  setPositions,
  getPositions,
  addPosition,
  updatePosition,
  deletePosition,
  setClosedPositions,
  getClosedPositions,
  addClosedPosition,
  setSummary,
  getSummary,
  addSummary,
  updateSummary,
  deleteSummary,
};
