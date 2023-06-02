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

const { timeToString } = require('../utils/helper.js');

function genEqualPairs(colVals, { delimiter = ' AND ', quote = '`' }) {
  return colVals.map(cv => `${quote}${cv[0]}${quote}=${cv[1]}`).join(delimiter);
}

function genTimePair(col) {
  return [col, "'" + timeToString() + "'"];
}

function retrieveValues(entity, cols, timeCol) {
  const colVals = [];
  cols.forEach(col => {
    const value = entity.valueOf(col);
    if (value != undefined) {
      colVals.push([col, value]);
    }
  });
  timeCol && colVals.push(genTimePair(timeCol));
  return colVals;
}

function createDatabase(schema, quote = '`') {
  return `CREATE DATABASE ${quote}${schema}${quote};`;
}

function select(entity, keys, quote = '`') {
  const keyPairs = genEqualPairs(retrieveValues(entity, keys), { quote });
  return `SELECT * FROM ${quote}${entity.tableName}${quote} WHERE ${keyPairs}`;
}

function insert(entity, cols, quote = '`') {
  const colVals = retrieveValues(entity, cols, 'created');
  const insCols = colVals.map(cv => quote + cv[0] + quote).join(',');
  const insVals = colVals.map(cv => cv[1]).join(',');
  return `INSERT INTO ${quote}${entity.tableName}${quote} (${insCols}) VALUES (${insVals})`;
}

function update(entity, keys, cols, quote = '`') {
  const keyPairs = genEqualPairs(retrieveValues(entity, keys), { quote });
  const setPairs = genEqualPairs(retrieveValues(entity, cols, 'modified'), {
    delimiter: ',',
    quote,
  });
  return `UPDATE ${quote}${entity.tableName}${quote} SET ${setPairs} WHERE ${keyPairs}`;
}

function insertOnDupKey(entity, keys, cols, quote = '`') {
  return (
    insert(entity, [...keys, ...cols], quote) +
    ' ON DUPLICATE KEY UPDATE ' +
    genEqualPairs(retrieveValues(entity, cols, 'modified'), {
      delimiter: ',',
      quote,
    })
  );
}

function insertOnConflict(entity, keys, cols, quote = '`') {
  return (
    insert(entity, [...keys, ...cols], quote) +
    ` ON CONFLICT (${keys.join(',')}) DO UPDATE SET ` +
    genEqualPairs(retrieveValues(entity, cols, 'modified'), {
      delimiter: ',',
      quote,
    })
  );
}

module.exports = {
  createDatabase,
  select,
  insert,
  update,
  insertOnDupKey,
  insertOnConflict,
};
