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
const process = require('node:process');
const { addons } = require('./app.json');
const buildConfig = require('./build-config.json');

const { targets } = buildConfig;
const [target] = process.argv.slice(2);
if (!target || !targets.includes(target)) {
  console.log(`Input a target: -- ${targets.join(' | ')}`);
  process.exit(1);
}

Object.assign(buildConfig.pkg, buildConfig[target]);
const { assets } = buildConfig.pkg;
const newAssets = [];
addons.includes.forEach(include => {
  newAssets.push(addons[include][target] + '/' + assets);
});
newAssets.push(...buildConfig.assets);
buildConfig.pkg.assets = newAssets;

fs.writeFileSync('build-config.json', JSON.stringify(buildConfig, null, '  '));
