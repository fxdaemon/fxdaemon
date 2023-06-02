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
const os = require('node:os');
const process = require('node:process');
const { createHash } = require('node:crypto');
const { rootName, addons } = require('../../app.json');
const { isPackaged } = require('../utils/helper.js');

function preprocess() {
  if (!isPackaged()) {
    return;
  }
  const pos = __dirname.indexOf(rootName);
  if (pos === -1) {
    return;
  }
  const baseDir = path.join(__dirname.substring(0, pos), rootName);

  const workDir = path.join(process.cwd(), addons.deployPath);
  if (!fs.existsSync(workDir)) {
    fs.mkdirSync(workDir);
  }

  addons.includes.forEach(include => {
    const addon = addons[include] || {};
    const source = path.join(baseDir, addon[os.platform()]);
    const files = fs.readdirSync(source);
    for (const file of files) {
      const curSource = path.join(source, file);
      const curTarget = path.join(workDir, file);
      if (fs.existsSync(curTarget)) {
        const curSourceContent = fs.readFileSync(curSource, {
          encoding: 'binary',
        });
        const curTargetContent = fs.readFileSync(curTarget, {
          encoding: 'binary',
        });
        const curSourceHash = createHash('sha256')
          .update(curSourceContent)
          .digest('hex');
        const curTargetHash = createHash('sha256')
          .update(curTargetContent)
          .digest('hex');
        if (curSourceHash === curTargetHash) {
          continue;
        }
      }
      fs.copyFileSync(curSource, curTarget);
    }

    const { symLinks } = addon;
    symLinks &&
      symLinks.forEach(([link, target]) => {
        const linkPath = path.join(workDir, link);
        const targetPath = path.join(workDir, target);
        if (fs.existsSync(targetPath) && !fs.existsSync(linkPath)) {
          fs.symlinkSync(targetPath, linkPath);
        }
      });
  });
}

module.exports = { preprocess };
