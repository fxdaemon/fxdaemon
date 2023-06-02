const dayjs = require('dayjs');
const utc = require('dayjs/plugin/utc');

function getMarketCloseTime() {
  dayjs.extend(utc);
  const now = dayjs();
  const weekend = now.add(5 - now.day(), 'd').format('YYYY-MM-DD');
  return dayjs.utc(`${weekend} ${_.getEndTradingDay()}`).unix();
}

function getCross(fast, slow) {
  let ud = 0;
  const count = fast.length;
  const endPos = count - 1 - fromPos;
  let i = endPos;
  for (; i > 0; i--) {
    if (fast[i - 1] <= slow[i - 1] && fast[i] > slow[i]) {
      ud = 1;
      break;
    } else if (fast[i - 1] >= slow[i - 1] && fast[i] < slow[i]) {
      ud = -1;
      break;
    }
  }
  const ret = { ud };
  if (ud != 0) {
    ret.crossPos = count - 1 - i;
    ret.crossFast = fast[i - 1];
    ret.crossSlow = slow[i - 1];
    ret.backFast = fast[endPos];
    ret.backSlow = slow[endPos];
  }
  return ret;
}

function init() {}

function main(symbol, period) {
  // Open a postion
  const macd = _.TA.MACD(symbol, period, 12, 6, 9);
  const ret = getCross(macd.macd, macd.signal);
  if (ret.ud != 0) {
    const { completed, order_id, error } = _.openPosition(
      '1234567',
      symbol,
      ud === 1 ? 'B' : 'S',
      1000,
      0,
      0,
    );
    if (completed) {
      _.logger.info(order_id);
    } else {
      _.logger.error(error);
    }
  }

  // Terminate the application before the market closes on Friday
  const marketCloseTime = getMarketCloseTime() - 3600;
  const dt = dayjs(_.getServerTime() * 1000);
  if (dt.unix() >= marketCloseTime) {
    _.emitter.emit('exit');
  }
}

function tick() {}

function trailmove() {}

function exit() {}

module.exports = { init, main, tick, trailmove, exit };
