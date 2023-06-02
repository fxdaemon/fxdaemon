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

const EventEmitter = require('node:events');
const process = require('node:process');
const parseArgs = require('minimist');
const log4js = require('log4js');
const talib = require('@fxdaemon/talibjs');
const appJson = require('./app.json');
const constants = require('./src/global/constants.js');
const { setGlobal } = require('./src/global/global.js');
const router = require('./src/run/router.js');
const defaultMain = require('./src/scripts/main.js');
const serializer = require('./src/storage/serialize.js');
const { preprocess } = require('./src/utils/bootstrap.js');
const { parseArgv, retrieveOption } = require('./src/utils/parse.js');
const { objectFilter, fullPathOnWorkDir } = require('./src/utils/helper.js');
const ta = require('./src/utils/ta.js');
const { help, version } = require('./src/utils/help.js');

function initialize(argv) {
  global.__DEV__ = process.env.NODE_ENV !== 'production';
  global.__CB__ = defaultMain;

  const config = parseArgv(argv, appJson);
  log4js.configure(config.logger);
  setGlobal('logger', log4js.getLogger(appJson.name));
  _.logger.level = config.logger.level || (__DEV__ ? 'debug' : 'info');

  setGlobal('config', config);
  setGlobal('emitter', new EventEmitter());
  setGlobal('talib', talib);
  setGlobal('TA', ta);
}

function loadScripts(scriptPath) {
  if (!scriptPath) return;
  try {
    const mainDefs = Object.keys(defaultMain);
    const scriptMains = objectFilter(
      require(fullPathOnWorkDir(scriptPath)),
      (key, value) => mainDefs.includes(key) && typeof value === 'function',
    );
    Object.assign(defaultMain, scriptMains);
  } catch (e) {
    _.logger.error(`Cannot load '${scriptPath}'`, e);
    process.exit(1);
  }
}

async function start([scriptPath]) {
  const { config } = _;
  if (retrieveOption(config, constants.ARGV_HELP_OPT)) {
    help();
    return;
  }
  if (retrieveOption(config, constants.ARGV_VERSION_OPT)) {
    version(appJson);
    return;
  }

  const route = router[retrieveOption(config, constants.ARGV_RUN_OPT)];
  if (route) {
    preprocess();

    if (
      !(await serializer.open(
        retrieveOption(config, constants.ARGV_STORAGE_OPT),
      ))
    ) {
      process.exit(1);
    }

    const runner = route();
    runner.init(config);
    runner.start(() => process.exit(1));

    const exit = () => runner.stop().then(serializer.close);
    process.on('SIGINT', exit);
    process.on('SIGQUIT', exit);
    _.emitter.on('exit', exit);
  }

  await talib.load();
  loadScripts(scriptPath);
}

const argv = parseArgs(process.argv.slice(2));
initialize(argv);
start(argv._);
