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

const drivers = require('./drivers/index.js');

let serializer;

async function open(storage) {
  if (storage) {
    const conns = Object.fromEntries(storage.split(';').map(v => v.split('=')));
    const driver = drivers[conns.driver];
    if (driver) {
      serializer = driver();
      if (!(await serializer.open(conns))) {
        serializer = null;
        _.logger.error(`Failed to open ${conns.driver}.`);
        return false;
      }
    } else {
      const availableDrivers = Object.keys(drivers).join(' | ');
      _.logger.error(
        `Cannot find '${conns.driver}'. Available drivers: [ ${availableDrivers} ]`,
      );
      return false;
    }
  }
  return true;
}

function find(entity) {
  return serializer && serializer.find(entity);
}

function insert(entity) {
  return serializer && serializer.insert(entity);
}

function update(entity) {
  return serializer && serializer.update(entity);
}

function remove(entity) {
  return serializer && serializer.remove(entity);
}

function insertOnConflict(entity) {
  return serializer && serializer.insertOnConflict(entity);
}

function close() {
  serializer && serializer.close();
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
