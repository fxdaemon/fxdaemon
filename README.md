## Features
Fxdaemon is an easy-to-use, lightweight and high-performance automated trading platform based on Node.js.

- The system can continuously monitor the financial market around the clock and use trading strategies to predict commodity price fluctuations, thereby realizing automatic trading for various commodities.
- The trading strategy programming language uses JavaScript, which can quickly execute automated trading.
- Starting from CUI, one process is allowed to be run in one account.
- Connect with the broker's trading server through the addon.
- The standard technical indicators based on TA-Lib are provided to users in global object.
- Transaction data can be saved to the database.
- Running on both Windows and Linux.

## Building

If the source has been modified, rebuild with reference to the following.

- Prerequisites
```bash
npm install -g pkg @vercel/ncc node-gyp
```

- forexapi
```bash
cd addons\forexapi
npm update
node-gyp configure
node-gyp build
```
```
Building a Linux addon, you need to switch to the Linux build environment.
```

- fxdaemon
```bash
cd node
npm update
npm run build -- win32  (Or: linux)
```

Bundle the strategy scripts with the following command.
```bash
npm run build:scripts sample.js
```

## Starting

The addons and libraries are deployed in the libs folder when starting up.

- windows
```bash
fxdaemon.exe --config sample.config sample.js
```

- linux
```bash
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:[install_directory]/libs
fxdaemon --config sample.config sample.js
```

## Usage

Run fxdaemon --help without arguments to see list of options:
```
fxdaemon [options] <strategy>

  Options:

    -h, --help           output usage information
    -v, --version        output fxdaemon version
    -c, --config         configuration file
    -s, --storage        database connection string
    -r, --run            running addon
```

- You can refer to example/sample.config for configuration settings.
- The storage and run options can also be set in the configuration file.
- If no configuration file is specified, "fxdaemon.config" in the same folder is read as default.
- Refer to [Log4js - Appenders](https://log4js-node.github.io/log4js-node/appenders.html) for logger settings.
- Except for the logger, other configuration settings can also be specified by command options.
- 3 storage drivers are available, the connection string is set as follows:
```
"driver=mysql;host=127.0.0.1;port=3306;user=root;password=123;database=fxdaemon"
"driver=pgsql;host=127.0.0.1;port=5432;user=postgres;password=123;database=fxdaemon"
"driver=sqlite;database=fxdaemon.db"
```

## Event handler

The following functions defined in the strategy scripts will be called when an event happens.

### &bull; init()

Called during initialization, one time only.

### &bull; main(symbol, period)

The timing of the invocation is when a bar history is up to date.
The symbol and period passed are defined as symbols and periods in the configuration settings.

### &bull; tick(offer)

when prices change.
```
offer fields: { 'symbol', 'time', 'bid', 'ask', 'high', 'low', 'volume' }
```

### &bull; trailmove(position)

when the PL of a position held changes.
```
position fields: { 'account_id', 'trade_id', 'offer_id', 'amount', 'side', 'open_rate', 'open_time', 'used_margin', 'pl', 'gross_pl', 'close', 'stop', 'limit', 'high', 'low', 'open_quote_id', 'open_order_id', 'open_order_req_id', 'commission', 'rollover_interest', 'trade_id_origin' }
```

### &bull; exit()

when the application is terminated.

## Global object

Expose system internal functions to strategy scripts as global object.

### &bull; _.config
Parameters set in command options and configuration file.

### &bull; _.emitter
The application can be terminated with the "emitter".
```javascript
_.emitter.emit('exit');
```

### &bull; _.logger
log4js objcet, refer [log4js-node](https://github.com/log4js-node/log4js-node) for usage.
```javascript
_.logger.info('Close position for expired.');
```

### &bull; _.talib
talibjs object, refer [here](https://github.com/fxdaemon/talibjs) for usage.

### &bull; _.TA
Wrapper for talib, 4 functions available.
```javascript
const { startTime, real } = _.TA.EMA('USD/JPY', 'm5', 30);
const { startTime, real } = _.TA.SMA('USD/JPY', 'm5', 200);
const { startTime, macd, signal, hist } = _.TA.MACD('USD/JPY', 'm5', 12, 6, 9);
const { startTime, slowk, slowd } = _.TA.STOCH('USD/JPY', 'm5', 7, 3, 'EMA', 3, 'EMA');
```

### &bull; _.getServerTime()
Get the current server time, returns the unix timestamp.
```javascript
const serverTime = _.getServerTime();
```

### &bull; _.getEndTradingDay()
The time of the end of a trading day in the format hh:mm:ss (UTC time zone).
```javascript
const endTradingDay = _.getEndTradingDay();
```

### &bull; _.getHistoricalPrices(symbol, period, startTime, endTime, callback)
Get historical prices asynchronous. "startTime" and "endTime" are the unix timestamp.
```javascript
_.getHistoricalPrices('USD/JPY', 'm5', 1685042600, 1685062600)
  .then(data =>
    _.logger.debug('getHistoricalPrices', data[0], data[data.length - 1]),
  )
  .catch(e => _.logger.error(e));
```

### &bull; _.openPosition(accountID, symbol, bs, amount, stop, limit)
Open position at market price. If stop and limit are set to 0, stop and limit orders are not placed.
```javascript
const {completed, order_id, error} = _.openPosition('1234567', 'USD/JPY', 'B', 1000, 138, 142);
if (completed) {
  _.logger.info(order_id);
} else {
  _.logger.error(error);
}
```

### &bull; _.changeStopLoss(accountID, tradeID, amount, stop)
Set the stop in an open position.
```javascript
const {completed, request_id, error} = _.changeStopLoss('1234567', '81896584', 137.5);
if (completed) {
  _.logger.info(request_id);
} else {
  _.logger.error(error);
}
```

### &bull; _.changeTakeProfit(accountID, tradeID, amount, limit)
Set the limit in an open position.
```javascript
const {completed, request_id, error} = _.changeTakeProfit('1234567', '81896584', 142.5);
if (completed) {
  _.logger.info(request_id);
} else {
  _.logger.error(error);
}
```

### &bull; _.closePosition(accountID, tradeID, amount)
Close a position.
```javascript
const {completed, request_id, error} = _.closePosition('1234567', '81896584', 1000);
if (completed) {
  _.logger.info(request_id);
} else {
  _.logger.error(error);
}
```

### &bull; _.closeAllPositions(accountID, symbol)
Close all positions in a specified symbol. If symbol is empty, all positions on this account will be closed.
```javascript
const {completed, request_ids, error} = _.closeAllPositions('1234567', '');
if (completed) {
  _.logger.info(request_ids);
} else {
  _.logger.error(error);
}
```

### &bull; _.getOffer(symbol)
Get an offer that contains information about current prices, and high/low trading day prices.
```javascript
const offer = _.getOffer('USD/JPY');
```

### &bull; _.getPrices(symbol, period, priceTypes)
Get historical prices in specified price types. 
```javascript
const [askOpen, askClose] = _.getPrices('USD/JPY', 'm5', ['ask_open', 'ask_close']);
```
available values for priceTypes:
```
ask_open, ask_high, ask_low, ask_close, ask_median, ask_typical, ask_weighted,
bid_open, bid_high, bid_low, bid_close, bid_median, bid_typical, bid_weighted
```
```
median = (high + low) / 2
typical = (high + low + close) / 3
weighted = (high + low + 2 * close) / 4
```

### &bull; _.getCandles(symbol, period)
Get historical prices.
```javascript
const candles = _.getCandles('USD/JPY', 'm5');
```

### &bull; _.getAccounts()
Get an account list.
```javascript
const accounts = _.getAccounts();
for (const account of accounts) {
  _.logger.info(accounts);
}
```

### &bull; _.getOrders()
Get an order list.
```javascript
const orders = _.getOrders();
for (const order of orders) {
  _.logger.info(orders);
}
```

### &bull; _.getPositions
Get an open position list.
```javascript
const positions = _.getPositions();
for (const position of positions) {
  _.logger.info(position);
}
```

### &bull; _.getClosedPositions
Get a closed position list.
```javascript
const closedPositions = _.getClosedPositions();
for (const closedPosition of closedPositions) {
  _.logger.info(closedPosition);
}
```

### &bull; _.getSummary
Get a summary list. It contains summarized information such as the average entry price, profit/loss, and so on.
```javascript
const summaries = _.getSummary();
for (const summary of summaries) {
  _.logger.info(summary);
}
```

## About addons development
Depending on requirements, it may be necessary to develop addons to connect to the trading server. Refer to forexapi for details of how to develop. Finally, the addons section of app.json needs to be modified to deploy.

## License
Fxdaemon is licensed under a [Apache License 2.0](./LICENSE).
