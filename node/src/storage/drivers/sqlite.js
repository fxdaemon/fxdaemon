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

const path = require('node:path');
const process = require('node:process');
const sqlite3 = require('sqlite3');
const { readSqlFile } = require('../../utils/helper.js');
const sql = require('../sql.js');

let db;

async function open({ database } = {}) {
  if (!database) {
    _.logger.error('Database path is required');
    return false;
  }
  db = new sqlite3.Database(path.join(process.cwd(), database));
  let succeed = true;
  const sqls = readSqlFile(path.join(__dirname, '../../assets/ddl/sqlite.sql'));
  for (const sql of sqls) {
    await run(sql).then(ret => (succeed = ret));
    if (!succeed) break;
  }
  return succeed;
}

function run(sql) {
  return new Promise(resolve => {
    db.run(sql, err => {
      if (err) {
        _.logger.error(err);
        resolve(false);
      } else {
        _.logger.debug(sql);
        resolve(true);
      }
    });
  });
}

function find(entity) {
  return new Promise(resolve => {
    db.all(entity.selSql(), (err, rows) => {
      if (err) {
        _.logger.error(err);
      } else {
        _.logger.debug(sql);
        resolve(rows);
      }
    });
  });
}

function insert(entity) {
  return run(entity.insSql());
}

function update(entity) {
  return run(entity.updSql());
}

function remove(entity) {
  return run(entity.delSql());
}

function insertOnConflict(entity) {
  return run(entity.insertOnConflict());
}

function close() {
  _.logger.info('Disconnect from sqlite');
  db.close();
}

module.exports = {
  open,
  find,
  insert,
  update,
  remove,
  insertOnConflict,
  close,
};
