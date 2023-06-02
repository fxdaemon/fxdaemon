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

const fs = require('fs');
const constants = require('../global/constants.js');
const { objectFilter, fullPathOnWorkDir } = require('./helper.js');

function loadConfigFile(path, appName) {
  const configFile = fullPathOnWorkDir(
    path || appName + constants.CONFIG_FILE_SUFFIX,
  );
  if (fs.existsSync(configFile)) {
    return JSON.parse(fs.readFileSync(configFile, 'utf-8'));
  }
  return {};
}

function handleArray(param) {
  if (param.length && param.startsWith('[') && param.endsWith(']')) {
    return param.slice(1, -1).split(',');
  }
  return param;
}

function parseArgv(argv, appJson) {
  const { ARGV_CONFIG_OPT } = constants;
  const excludeOptions = ['_', ARGV_CONFIG_OPT, ARGV_CONFIG_OPT.slice(0, 1)];
  const argvOptions = objectFilter(
    argv,
    key => !excludeOptions.includes(key),
    handleArray,
  );

  const config = loadConfigFile(
    retrieveOption(argv, ARGV_CONFIG_OPT),
    appJson.name,
  );
  const appJsonLogger = appJson.logger;
  const configLogger = config.logger;
  if (configLogger) {
    const appenders = configLogger.appenders || {};
    appJsonLogger.appenders = Object.assign(appenders, appJsonLogger.appenders);
    appJsonLogger.categories.default.appenders = Object.keys(
      appJsonLogger.appenders,
    );
    if (configLogger.level) {
      appJsonLogger.level = configLogger.level;
    }
  }
  config.logger = appJsonLogger;

  return Object.assign(config, argvOptions);
}

function retrieveOption(argv, name) {
  return argv[name] || argv[name.slice(0, 1)];
}

module.exports = { parseArgv, retrieveOption };
