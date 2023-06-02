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

const fs = require('node:fs');
const path = require('node:path');
const process = require('node:process');
const dayjs = require('dayjs');
const { DATETIME_FORMAT } = require('../global/constants.js');

function toArray(v) {
  if (Array.isArray(v)) {
    return v;
  } else if (typeof v === 'string') {
    if (v.length && v.startsWith('[') && v.endsWith(']')) {
      return v.slice(1, -1).split(',');
    } else {
      return v.split(',');
    }
  }
  return [];
}

function objectFilter(obj, predicate, handleValue) {
  return Object.fromEntries(
    Object.keys(obj)
      .filter(key => predicate(key, obj[key]))
      .map(key => [key, handleValue ? handleValue(obj[key]) : obj[key]]),
  );
}

function fullPathOnWorkDir(fileName) {
  return path.isAbsolute(fileName)
    ? fileName
    : path.join(process.cwd(), fileName);
}

function readSqlFile(sqlFile) {
  const sqls = [];
  let sql = '';
  fs.readFileSync(sqlFile, 'utf-8')
    .split(/\r?\n/)
    .forEach(line => {
      line = line.trim();
      if (line.endsWith(';')) {
        sqls.push(sql + line);
        sql = '';
      } else {
        sql += line;
      }
    });
  return sqls;
}

function timeToString(time) {
  return time === undefined
    ? dayjs().format(DATETIME_FORMAT)
    : dayjs(time * 1000).format(DATETIME_FORMAT);
}

function isPackaged() {
  return /^\/snapshot\//.test(__dirname) || /^.:\\snapshot\\/.test(__dirname);
}

module.exports = {
  toArray,
  objectFilter,
  fullPathOnWorkDir,
  readSqlFile,
  timeToString,
  isPackaged,
};
