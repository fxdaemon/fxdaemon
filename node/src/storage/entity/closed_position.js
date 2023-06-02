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

const keys = ['account_id', 'trade_id'];

const cols = [
  'offer_id',
  'amount',
  'side',
  'open_rate',
  'open_time',
  'pl',
  'net_pl',
  'gross_pl',
  'open_quote_id',
  'open_order_id',
  'open_order_req_id',
  'close_rate',
  'close_time',
  'close_quote_id',
  'close_order_id',
  'close_order_req_id',
  'commission',
  'rollover_interest',
  'trade_id_origin',
  'trade_id_remain',
];

module.exports = class ClosedPosition extends BaseEntity {
  tableName = 'closed_position';

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
    if (name === 'open_time' || name === 'close_time') {
      return super.valueOfTime(name);
    } else {
      return super.valueOf(name);
    }
  }
};
