#!/bin/bash
set -e

echo "Starting build process..."

clang compile.c -c -o out/compile.o -I./adaptors

clang parse.c -c -o out/parse.o -I./adaptors

# clang out/compile.o  out/parse.o -o out/compile


echo "Build process completed successfully."