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

const dataset = require('../../storage/dataset.js');
const serializer = require('../../storage/serialize.js');
const { objectFilter } = require('../../utils/helper.js');

const ADDED = 0;
const CHANGED = 1;
const DELETED = 2;

const onEvents = {
  Info: onInfo,
  Error: onError,
  Session: onSession,
  Offer: onOffer,
  Candle: onCandle,
  Account: onAccount,
  Order: onOrder,
  Trade: onTrade,
  ClosedTrade: onClosedTrade,
  Message: onMessage,
  Summary: onSummary,
};

function onInfo(info) {
  _.logger.info('Info:', info);
}

function onError(error) {
  _.logger.error('Error:', error);
}

function onSession(status) {
  _.logger.info('Session:', status);
}

function onOffer(offer) {
  const cols = ['symbol', 'time', 'bid', 'ask', 'high', 'low', 'volume'];
  const validOffer = objectFilter(offer, (k, v) => cols.includes(k) && v);
  dataset.updateOffer(validOffer);
  __CB__.tick(validOffer);
}

function onCandle(candle) {
  const { symbol, period } = candle;
  dataset.addCandle(symbol, period, candle);
  _.logger.debug(candle);
  __CB__.main(symbol, period);
}

function onAccount(account) {
  dataset.updateAccount(account);
}

function onOrder(order) {
  if (order.status === ADDED) {
    const newOrder = dataset.addOrder(order);
    serializer.insert(newOrder);
  } else if (order.status === CHANGED) {
    const [, newOrder] = dataset.updateOrder(order) || [];
    newOrder && serializer.update(newOrder);
  }
}

function onTrade(trade) {
  trade.high = trade.low = trade.close;
  if (trade.status === ADDED) {
    const newPositon = dataset.addPosition(trade);
    serializer.insert(newPositon);
  } else if (trade.status === CHANGED) {
    const [prevPositon, newPositon] = dataset.updatePosition(trade) || [];
    newPositon && newPositon.setHighLow(prevPositon);
    __CB__.trailmove(trade);
  } else if (trade.status === DELETED) {
    const lastPosition = dataset.deletePosition(trade);
    lastPosition && serializer.update(lastPosition);
  }
}

function onClosedTrade(closedTrade) {
  const newClosedPosition = dataset.addClosedPosition(closedTrade);
  serializer.insert(newClosedPosition);
}

function onMessage(message) {
  _.logger.info('Message:', message);
}

function onSummary(summary) {
  if (summary.status === ADDED) {
    dataset.addSummary(summary);
  } else if (summary.status === CHANGED) {
    dataset.updateSummary(summary);
  } else if (summary.status === DELETED) {
    dataset.deleteSummary(summary);
  }
}

function handleEvent(type, data) {
  onEvents[type](data);
}

module.exports = { handleEvent };
