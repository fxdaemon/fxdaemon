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

const { timeToString } = require('../../utils/helper.js');

module.exports = class BaseEntity {
  constructor(object) {
    Object.assign(this, object);
  }

  valueOf(name) {
    const value = this[name];
    return typeof value === 'string' ? "'" + value + "'" : value;
  }

  valueOfTime(name) {
    const value = this[name];
    if (value !== undefined) {
      return "'" + timeToString(this[name]) + "'";
    }
  }
};
