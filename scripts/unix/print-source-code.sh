#!/bin/bash

# Need at least one path + output file
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <path> [<path> ...] <output-file>"
    exit 1
fi

# Last argument = output file
OUTFILE="${@: -1}"

# All but last = paths
PATHS=("${@:1:$#-1}")

cd "$(dirname "$0")"
cd ../..

# Clear/create output file
> "$OUTFILE"

for path in "${PATHS[@]}"; do
    find "$path" -type f \( -name '*.cpp' -o -name '*.hpp' \)
done | while IFS= read -r file; do
    echo "$file" >> "$OUTFILE"
    cat "$file" >> "$OUTFILE"
done