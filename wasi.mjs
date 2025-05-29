#!/usr/bin/env node

import { readFileSync } from "fs";
import { WASI } from "wasi";
import path from "path";

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
  const wasm = await WebAssembly.compile(
    readFileSync(
      path.join(
        typeof __dirname !== "undefined" ? __dirname : import.meta.dirname,
        "sslc.wasm"
      )
    )
  );
  const instance = await WebAssembly.instantiate(wasm, wasi.getImportObject());

  wasi.start(instance);
})();
