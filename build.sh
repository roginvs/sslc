#!/bin/bash
set -e

echo "Starting build process..."

FILES="compile parse parselib extra gencode lex parseext  
mcpp_main mcpp_directive mcpp_eval mcpp_expand mcpp_support mcpp_system 
optimize
"

OUT=""

for f in $FILES; do
    if [ ! -f "$f.c" ]; then
        echo "Error: $f.c not found!"
        exit 1
    fi
    echo "=== Building $f.c ==="
    gcc -m32 -g -O0  $f.c -c -o out/$f.o -I./adaptors

    OUT="$OUT out/$f.o"

    echo ""
    echo ""
done

echo "=== Linking ==="

gcc -m32  -g -O0  $OUT -o out/compile


echo "Build process completed successfully."