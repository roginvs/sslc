#!/usr/bin/env node

import { readFileSync } from "fs";
import { WASI } from "wasi";
import path from "path";
import { fileURLToPath } from "url";

process.removeAllListeners("warning").on("warning", (err) => {
  if (
    err.name !== "ExperimentalWarning" &&
    !err.message.includes("experimental")
  ) {
    console.warn(err);
  }
});

const wasi = new WASI({
  version: "preview1",
  args: process.argv.slice(1),
  env: process.env,
  preopens: {
    "/": "/",
  },
});

(async () => {
  const dirname =
    typeof __dirname !== "undefined"
      ? __dirname
      : path.dirname(fileURLToPath(import.meta.url));

  const wasm = await WebAssembly.compile(
    readFileSync(path.join(dirname, "sslc.wasm"))
  );
  const instance = await WebAssembly.instantiate(wasm, wasi.getImportObject());

  const returnCode = wasi.start(instance);
  process.exit(returnCode);
})();
