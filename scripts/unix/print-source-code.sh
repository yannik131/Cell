#!/bin/bash

# Fail on missing argument
if [ -z "$1" ]; then
    echo "Usage: $0 <output-file>"
    exit 1
fi

OUTFILE="$1"

cd "$(dirname "$0")"
cd ../..

# Clear/create output file
> "$OUTFILE"

# Find only .cpp and .hpp files recursively
find src -type f \( -name '*.cpp' -o -name '*.hpp' \) | \
while IFS= read -r file; do
    echo "$file" >> "$OUTFILE"
    cat "$file" >> "$OUTFILE"
done
