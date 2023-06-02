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

function shift(ary, outBegIdx, outNBElement) {
  for (let i = outNBElement - 1; i >= 0; i--) {
    ary[outBegIdx + i] = ary[i];
  }
  for (let i = 0; i < outBegIdx; i++) {
    ary[i] = 0;
  }
}

function SMA(symbol, period, timePeriod = 30) {
  const [startTime, inReal] = _.getPrices(symbol, period, [
    'start_time',
    'ask_close',
  ]);
  const {
    error,
    outReal: real,
    outBegIdx,
    outNBElement,
  } = _.talib.TA('SMA', {
    startIdx: 0,
    endIdx: inReal.length - 1,
    inReal,
    optInTimePeriod: timePeriod,
  });
  if (error) {
    _.logger.error(error.message);
  } else {
    shift(startTime, outBegIdx, outNBElement);
    shift(real, outBegIdx, outNBElement);
    return { startTime, real };
  }
}

function EMA(symbol, period, timePeriod = 30) {
  const [startTime, inReal] = _.getPrices(symbol, period, [
    'start_time',
    'ask_close',
  ]);
  const {
    error,
    outReal: real,
    outBegIdx,
    outNBElement,
  } = _.talib.TA('EMA', {
    startIdx: 0,
    endIdx: inReal.length - 1,
    inReal,
    optInTimePeriod: timePeriod,
  });
  if (error) {
    _.logger.error(error.message);
  } else {
    shift(startTime, outBegIdx, outNBElement);
    shift(real, outBegIdx, outNBElement);
    return { startTime, real };
  }
}

function MACD(
  symbol,
  period,
  fastPeriod = 12,
  slowPeriod = 26,
  signalPeriod = 9,
) {
  const [startTime, inReal] = _.getPrices(symbol, period, [
    'start_time',
    'ask_close',
  ]);
  const {
    error,
    outMACD: macd,
    outMACDSignal: signal,
    outMACDHist: hist,
    outBegIdx,
    outNBElement,
  } = _.talib.TA('MACD', {
    startIdx: 0,
    endIdx: inReal.length - 1,
    inReal,
    optInFastPeriod: fastPeriod,
    optInSlowPeriod: slowPeriod,
    optInSignalPeriod: signalPeriod,
  });
  if (error) {
    _.logger.error(error.message);
  } else {
    shift(startTime, outBegIdx, outNBElement);
    shift(macd, outBegIdx, outNBElement);
    shift(signal, outBegIdx, outNBElement);
    shift(hist, outBegIdx, outNBElement);
    return { startTime, macd, signal, hist };
  }
}

function STOCH(
  symbol,
  period,
  fastKPeriod = 5,
  slowKPeriod = 3,
  slowKMAType = 'EMA',
  slowDPeriod = 3,
  slowDMAType = 'EMA',
) {
  const [startTime, high, low, close] = _.getPrices(symbol, period, [
    'start_time',
    'ask_high',
    'ask_low',
    'ask_close',
  ]);
  const {
    error,
    outSlowK: slowk,
    outSlowD: slowd,
    outBegIdx,
    outNBElement,
  } = _.talib.TA('STOCH', {
    startIdx: 0,
    endIdx: high.length - 1,
    high,
    low,
    close,
    optInFastK_Period: fastKPeriod,
    optInSlowK_Period: slowKPeriod,
    optInSlowK_MAType: slowKMAType,
    optInSlowD_Period: slowDPeriod,
    optInSlowD_MAType: slowDMAType,
  });
  if (error) {
    _.logger.error(error.message);
  } else {
    shift(startTime, outBegIdx, outNBElement);
    shift(slowk, outBegIdx, outNBElement);
    shift(slowd, outBegIdx, outNBElement);
    return { startTime, slowk, slowd };
  }
}

module.exports = { SMA, EMA, MACD, STOCH };
