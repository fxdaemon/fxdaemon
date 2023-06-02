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

const BaseEntity = require('./base_entity.js');
const sql = require('../sql.js');

const keys = ['account_id', 'order_id'];

const cols = [
  'request_id',
  'offer_id',
  'trade_id',
  'side',
  'stage',
  'type',
  'order_status',
  'amount',
  'rate',
  'execution_rate',
  'rate_min',
  'rate_max',
  'stop',
  'limit',
  'status_time',
  'primary_id',
  'origin_amount',
  'filled_amount',
];

module.exports = class Order extends BaseEntity {
  tableName = 'order';

  selSql(quote = '`') {
    return sql.select(this, keys, quote);
  }

  insSql(quote = '`') {
    return sql.insert(this, [...keys, ...cols], quote);
  }

  updSql(quote = '`') {
    return sql.update(this, keys, cols, quote);
  }

  insOnDupKeySql(quote = '`') {
    return sql.insertOnDupKey(this, keys, cols, quote);
  }

  insertOnConflict(quote = '`') {
    return sql.insertOnConflict(this, keys, cols, quote);
  }

  valueOf(name) {
    if (name === 'status_time' || name === 'expire_time') {
      return super.valueOfTime(name);
    } else {
      return super.valueOf(name);
    }
  }
};
