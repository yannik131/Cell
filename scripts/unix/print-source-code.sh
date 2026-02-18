#!/bin/bash

# Fail on missing argument
if [ -z "$2" ]; then
    echo "Usage: $0 <path> <output-file>"
    exit 1
fi

OUTFILE="$2"

cd "$(dirname "$0")"
cd ../..

# Clear/create output file
> "$OUTFILE"

# Find only .cpp and .hpp files recursively
find "$1" -type f \( -name '*.cpp' -o -name '*.hpp' \) | \
while IFS= read -r file; do
    echo "$file" >> "$OUTFILE"
    cat "$file" >> "$OUTFILE"
done
