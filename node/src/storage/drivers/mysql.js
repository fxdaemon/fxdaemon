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
const mysql = require('mysql');
const { readSqlFile } = require('../../utils/helper.js');
const sql = require('../sql.js');

const conns = {};

async function open({ host, port = 3306, user, password, database } = {}) {
  let connected = false;
  conns.params = { host, port, user, password, database };
  await connect(host, port, user, password)
    .then(() => checkDatabaseExists(database))
    .then(exists => !exists && createDatabase(database))
    .then(() => selectDatabase(database))
    .then(() => checkTableExists('offer'))
    .then(exists => !exists && createTables())
    .then(() => (connected = true))
    .catch(e => _.logger.error(e));
  handleError();
  return connected;
}

function connect(host, port, user, password) {
  _.logger.info('Connect to mysql');
  return new Promise((resolve, reject) => {
    conns.connection = mysql.createConnection({
      host,
      port,
      user,
      password,
    });
    conns.connection.connect(err => {
      if (err) {
        reject(err);
      } else {
        _.logger.info('connected as id ' + conns.connection.threadId);
        resolve();
      }
    });
  });
}

function handleError() {
  conns.connection.on('error', err => {
    _.logger.error(err);
    if (err.fatal) {
      tryConnect();
    }
  });
}

function tryConnect() {
  _.logger.info('Reconnect to mysql');
  conns.connection = mysql.createConnection(conns.params);
  conns.connection.connect(err => {
    if (err) {
      _.logger.error(err);
      setTimeout(tryConnect, 3000);
    } else {
      _.logger.info('connected as id ' + conns.connection.threadId);
      handleError();
    }
  });
}

function checkDatabaseExists(database) {
  return new Promise(resolve => {
    conns.connection.query(
      'SELECT schema_name FROM information_schema.schemata ' +
        `WHERE schema_name = '${database}';`,
      (err, results) => {
        if (err) {
          reject(err);
        } else {
          resolve(results.length > 0);
        }
      },
    );
  });
}

function checkTableExists(tableName) {
  return new Promise(resolve => {
    conns.connection.query(`SELECT 1 FROM \`${tableName}\` LIMIT 1;`, err => {
      if (err) {
        resolve(false);
      } else {
        resolve(true);
      }
    });
  });
}

function createDatabase(database) {
  _.logger.info(`Create database "${database}"`);
  return new Promise((resolve, reject) => {
    conns.connection.query(sql.createDatabase(database), err => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

function selectDatabase(database) {
  return new Promise((resolve, reject) => {
    conns.connection.query(`use ${database}`, err => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

function createTables() {
  _.logger.info('Create tables');
  return new Promise((resolve, reject) => {
    let count = 0;
    const sqls = readSqlFile(
      path.join(__dirname, '../../assets/ddl/mysql.sql'),
    );
    for (const sql of sqls) {
      conns.connection.query(sql, err => {
        if (err) {
          reject(err);
        } else {
          count += 1;
          if (count === sqls.length) resolve();
        }
      });
    }
  });
}

function query(sql) {
  return new Promise(resolve => {
    conns.connection.query(sql, (err, results, fields) => {
      if (err) {
        _.logger.error(err);
      } else {
        _.logger.debug(sql);
        resolve(results, fields);
      }
    });
  });
}

function find(entity) {
  return query(entity.selSql());
}

function insert(entity) {
  return query(entity.insSql());
}

function update(entity) {
  return query(entity.updSql());
}

function remove(entity) {
  return query(entity.delSql());
}

function insertOnConflict(entity) {
  return query(entity.insOnDupKeySql());
}

function close() {
  _.logger.info('Disconnect from mysql');
  conns.connection.end(err => err && _.logger.error(err));
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
