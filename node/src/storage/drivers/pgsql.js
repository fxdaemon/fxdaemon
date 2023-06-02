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
const { Client } = require('pg');
const { readSqlFile } = require('../../utils/helper.js');
const sql = require('../sql.js');

const conns = {};

async function open({ host, port = 5432, user, password, database } = {}) {
  let connected = false;
  conns.params = { host, port, user, password, database };
  await connect(host, port, user, password)
    .then(() => checkDatabaseExists(database))
    .then(exists => !exists && createDatabase(database))
    .then(() => disconnect())
    .then(() => reconnect(host, port, user, password, database))
    .then(() => checkTableExists('offer'))
    .then(exists => !exists && createTables())
    .then(() => (connected = true))
    .catch(e => _.logger.error(e));
  handleError();
  return connected;
}

function connect(host, port, user, password) {
  _.logger.info('Connect to PostgreSQL');
  return new Promise((resolve, reject) => {
    conns.client = new Client({
      host,
      port,
      user,
      password,
    });
    conns.client.connect(err => {
      if (err) {
        reject(err);
      } else {
        _.logger.info('Successfully connected');
        resolve();
      }
    });
  });
}

function disconnect() {
  return new Promise((resolve, reject) => {
    conns.client.end(err => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

function reconnect(host, port, user, password, database) {
  _.logger.info('Reconnect to PostgreSQL');
  return new Promise((resolve, reject) => {
    conns.client = new Client({
      host,
      port,
      user,
      password,
      database,
    });
    conns.client.connect(err => {
      if (err) {
        reject(err);
      } else {
        _.logger.info('Successfully connected');
        resolve();
      }
    });
  });
}

function handleError() {
  conns.client.on('error', err => {
    _.logger.error(err);
    tryConnect();
  });
}

function tryConnect() {
  _.logger.info('Reconnect to PostgreSQL');
  conns.client = new Client(conns.params);
  conns.client.connect(err => {
    if (err) {
      _.logger.error(err);
      setTimeout(tryConnect, 3000);
    } else {
      _.logger.info('Successfully connected');
      handleError();
    }
  });
}

function checkDatabaseExists(database) {
  return new Promise(resolve => {
    conns.client.query(
      'SELECT datname FROM pg_catalog.pg_database ' +
        `WHERE lower(datname) = lower('${database}');`,
      (err, res) => {
        if (err) {
          reject(err);
        } else {
          resolve(res.rows.length > 0);
        }
      },
    );
  });
}

function checkTableExists(tableName) {
  return new Promise(resolve => {
    conns.client.query(`SELECT 1 FROM "${tableName}" LIMIT 1;`, err => {
      if (err) {
        resolve(false);
      } else {
        resolve(true);
      }
    });
  });
}

function createDatabase(database) {
  _.logger.info(`Create database ${database}`);
  return new Promise((resolve, reject) => {
    conns.client.query(sql.createDatabase(database, '"'), err => {
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
      path.join(__dirname, '../../assets/ddl/pgsql.sql'),
    );
    for (const sql of sqls) {
      conns.client.query(sql, err => {
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
    conns.client.query(sql, (err, res) => {
      if (err) {
        _.logger.error(err);
      } else {
        _.logger.debug(sql);
        resolve(res);
      }
    });
  });
}

function find(entity) {
  return query(entity.selSql('"'));
}

function insert(entity) {
  return query(entity.insSql('"'));
}

function update(entity) {
  return query(entity.updSql('"'));
}

function remove(entity) {
  return query(entity.delSql('"'));
}

function insertOnConflict(entity) {
  return query(entity.insertOnConflict('"'));
}

function close() {
  _.logger.info('Disconnect from PostgreSQL');
  conns.client.end(err => err && _.logger.error(err));
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
