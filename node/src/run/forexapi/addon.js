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

const os = require('node:os');
const path = require('node:path');
const process = require('node:process');
const { addons } = require('../../../app.json');
const { CANDLES_MAX_COUNT } = require('../../global/constants.js');
const { isPackaged } = require('../../utils/helper.js');

const moduleName = addons.forexapi.module;
const forexapi = isPackaged()
  ? require(path.join(process.cwd(), `${addons.deployPath}/${moduleName}`))
  : require(`../../../${addons.forexapi[os.platform()]}/${moduleName}`);

module.exports = class ForexApi {
  constructor({
    userName = '',
    password = '',
    host = '',
    connection = '',
    session_id = '',
    pin = '',
    symbols = [],
    periods = [],
  } = {}) {
    this.userName = userName;
    this.password = password;
    this.host = host;
    this.connection = connection;
    this.session_id = session_id;
    this.pin = pin;
    this.symbols = symbols;
    this.periods = periods;
    this.node = forexapi;
    this.runnable = true;
  }

  isRunnable() {
    return this.runnable;
  }

  start() {
    return new Promise((resolve, reject) => {
      const { init, login, close, loadDataAsync } = forexapi;
      const { session_id, pin } = this;
      const { completed, error: iniError } = init({ session_id, pin });
      if (!completed) {
        reject(new Error(iniError));
        return;
      }

      const { userName, password, host, connection } = this;
      const { connected, error: loginError } = login(
        userName,
        password,
        host,
        connection,
      );
      if (!connected) {
        close();
        reject(new Error(loginError));
        return;
      }

      const { symbols, periods } = this;
      loadDataAsync(
        { symbols, periods, priceHistoryCount: CANDLES_MAX_COUNT },
        resolve,
      );
    });
  }

  on(callback) {
    const { nextEvent } = forexapi;
    const isRunnable = this.isRunnable.bind(this);
    (function run() {
      setImmediate(function () {
        while (nextEvent(callback));
        isRunnable() && run();
      });
    })();
  }

  stop() {
    const { logout, close } = forexapi;
    logout();
    setTimeout(() => {
      this.runnable = false;
      close();
    }, 1000);
  }
};
