#!/usr/bin/env node

// TODO: Make this as "bin" in the package

import path from "path";
import Module from "../build/bin/sslc.mjs";
import fs from "fs";

/**
 *
 * @param {string[]} sslcArgs Command-line arguments to sslc
 * @param {Uint8Array} [wasmBinary] A compiled binary, if bundler fails to bundle .wasm file
 * @returns {{stdout: string, stderr: string, returnCode: number}}
 */
async function compile(sslcArgs, wasmBinary) {
  const stdout = [];
  const stderr = [];

  try {
    const instance = await Module({
      print: (text) => stdout.push(text),
      printErr: (text) => stderr.push(text),
      noInitialRun: true,
      ...(wasmBinary
        ? {
            wasmBinary,
            locateFile: (p) => p,
          }
        : {}),
    });

    instance.FS.mkdir("/host");

    const cwd = path.parse(process.cwd());

    instance.FS.mount(
      // Using NODEFS instead of NODERAWFS because
      // NODERAWFS caused errors when the same module
      // runs the second time
      instance.NODEFS,
      {
        root: cwd.root,
      },
      "/host"
    );
    instance.FS.chdir(path.join("host", cwd.dir, cwd.name));

    const returnCode = instance.callMain(sslcArgs);

    instance.FS.chdir("/");
    instance.FS.unmount("/host");

    return {
      returnCode,
      stdout: stdout.join("\n"),
      stderr: stderr.join("\n"),
    };
  } catch (e) {
    return {
      returnCode: 1,
      stdout: stdout.join("\n"),
      stderr: stderr.join("\n") + `\nERROR: ${e.name} ${e.message} ${e.stack}`,
    };
  }
}

const wasmBinary = fs.readFileSync( "../build/bin/sslc.wasm");

process.chdir("tmp/Fallout2_Restoration_Project/scripts_src");

for (const dir of fs
  .readdirSync(".")
  .filter((dir) => fs.statSync(dir).isDirectory())) {
    console.info(`Compiling scripts in ${dir}...`);
  process.chdir(dir);
  for (const fname of fs
    .readdirSync(".")
    .filter((file) => file.endsWith(".ssl"))) {
    console.log(`Compiling ${fname}...`);
    const opts = [
        ..."-q -p -l -O2 -d -s -n".split(" "),
        fname,
        "-o",
        fname.replace(/\.ssl$/, ".int"),
      ];
      console.info(` > sslc ${opts.join(" ")}`);
    const { stdout, stderr, returnCode } = await compile(opts,wasmBinary);
    console.info(` > returnCode: ${returnCode}`);
    if (returnCode !== 0) {
      console.info(stdout);
      console.error(stderr);

      process.exit(1);
    }

    await Promise.resolve();
    await new Promise(r => setTimeout(r, 15000))
    //console.log(stdout);
    //if (stderr) { console.error(stderr); }
    //process.exit(returnCode);
  }
  process.chdir("..");
}

/*
const { stdout, stderr, returnCode } = await compile(process.argv.slice(2))
console.log(stdout);
if (stderr) { console.error(stderr)};
process.exit(returnCode);
*/
